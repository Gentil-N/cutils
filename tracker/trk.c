#include "trk.h"

#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#ifndef TRK_MALLOC
#define TRK_MALLOC malloc
#endif

#ifndef TRK_FREE
#define TRK_FREE free
#endif

struct MemoryLabel
{
    void *ptr;
    const char *file;
    int line;
    struct MemoryLabel *prev;
    struct MemoryLabel *next;
};

static int INIT = 0;
static pthread_mutex_t MUTEX = {0};
static struct MemoryLabel *HEAD = NULL;
static struct MemoryLabel *TAIL = NULL;

int mutex_create_or_lock()
{
    if (INIT == 0 && pthread_mutex_init(&MUTEX, NULL) != 0) return 1;
    if (pthread_mutex_lock(&MUTEX) != 0) return 1;
    INIT = 1; // set after mutex locked!
    return 0;
}

int mutex_unlock()
{
    if (INIT == 1 && pthread_mutex_unlock(&MUTEX) != 0) return 1;
    return 0;
}

int mutex_unlock_and_destroy()
{
    if (INIT == 1)
    {
        INIT = 0;// assuming that MUTEX is already locked
        if (pthread_mutex_unlock(&MUTEX) != 0 || pthread_mutex_destroy(&MUTEX) != 0) return 1;
    }
    return 0;
}

struct MemoryLabel *create_memory_label_generic(void *ptr, const char *file, int line, struct MemoryLabel *prev, struct MemoryLabel *next)
{
    struct MemoryLabel *mem_label = TRK_MALLOC(sizeof(struct MemoryLabel));
    if (mem_label == NULL)
    {
        return NULL;
    }
    mem_label->ptr = ptr;
    mem_label->file = file;
    mem_label->line = line;
    mem_label->prev = prev;
    mem_label->next = next;
    return mem_label;
}

int create_memory_label(void *ptr, const char *file, int line)
{
    if (ptr == NULL)
    {
        return 0;
    }
    if (HEAD == NULL)
    {
        if ((HEAD = create_memory_label_generic(NULL, NULL, 0, NULL, NULL)) == NULL) return 1;
        TAIL = HEAD;
    }
    struct MemoryLabel *mem_label = create_memory_label_generic(ptr, file, line, TAIL, NULL);
    if (mem_label == NULL) return 1;
    TAIL->next = mem_label;
    TAIL = mem_label;
    return 0;
}

struct MemoryLabel *find_memory_label(void *ptr)
{
    if (HEAD == NULL) return NULL;
    struct MemoryLabel *current = HEAD->next;
    while (current)
    {
        if (current->ptr == ptr)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

enum TrkResult _trk_register(void *address, const char *file, int line)
{
    if (address == NULL) return TRK_RESULT_OK;
    if (mutex_create_or_lock() != 0) return TRK_RESULT_ERROR_INTERNAL_PTHREAD;
    if (create_memory_label(address, file, line) != 0) return TRK_RESULT_ERROR_OUT_OF_MEMORY;
    if (mutex_unlock() != 0) return TRK_RESULT_ERROR_INTERNAL_PTHREAD;
    return TRK_RESULT_OK;
}

enum TrkResult _trk_unregister(void *address)
{
    if (address == NULL) return TRK_RESULT_OK;
    if (mutex_create_or_lock() != 0) return TRK_RESULT_ERROR_INTERNAL_PTHREAD;
    struct MemoryLabel *mem_label;
    if ((mem_label = find_memory_label(address)) == NULL)
    {
        if (mutex_unlock() != 0) return TRK_RESULT_ERROR_INTERNAL_PTHREAD;
        return TRK_RESULT_ERROR_ADDRESS_NOT_FOUND;
    }
    mem_label->prev->next = mem_label->next;
    if (mem_label->next)
    {
        mem_label->next->prev = mem_label->prev;
    }
    else
    {
        TAIL = mem_label->prev;
    }
    TRK_FREE(mem_label);
    if (TAIL == HEAD)
    {
        TRK_FREE(HEAD);
        HEAD = NULL;
        TAIL = NULL;
        if (mutex_unlock_and_destroy() != 0) return TRK_RESULT_ERROR_INTERNAL_PTHREAD;
    }
    else
    {
        if (mutex_unlock() != 0) return TRK_RESULT_ERROR_INTERNAL_PTHREAD;
    }
    return TRK_RESULT_OK;
}

enum TrkResult _trk_change_register(void *old_address, void *new_address)
{
    if (old_address == NULL) return TRK_RESULT_OK;
    if (mutex_create_or_lock() != 0) return TRK_RESULT_ERROR_INTERNAL_PTHREAD;
    struct MemoryLabel *mem_label;
    if ((mem_label = find_memory_label(old_address)) == NULL)
    {
        if (mutex_unlock() != 0) return TRK_RESULT_ERROR_INTERNAL_PTHREAD;
        return TRK_RESULT_ERROR_ADDRESS_NOT_FOUND;
    }
    mem_label->ptr = new_address;
    if (mutex_unlock() != 0) return TRK_RESULT_ERROR_INTERNAL_PTHREAD;
    return TRK_RESULT_OK;
}

enum TrkResult _trk_trace(trk_trace_fn fn)
{
    if (fn == NULL) return TRK_RESULT_OK;
    if (mutex_create_or_lock() != 0) return TRK_RESULT_ERROR_INTERNAL_PTHREAD;
    if (HEAD == NULL)
    {
        if (mutex_unlock() != 0) return TRK_RESULT_ERROR_INTERNAL_PTHREAD;
        return TRK_RESULT_OK;
    }
    struct MemoryLabel *current = HEAD->next;
    while (current)
    {
        fn(current->ptr, current->file, current->line);
        current = current->next;
    }
    if (mutex_unlock() != 0) return TRK_RESULT_ERROR_INTERNAL_PTHREAD;
    return TRK_RESULT_OK;
}
