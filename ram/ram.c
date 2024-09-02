#include "ram.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

ERR_DECLARE(ram);

#define _ram_throw_error(code) assert(_ram_throw_error_fl(code, __FILE__, __LINE__) == 0)

typedef struct MemoryLabel MemoryLabel;

struct MemoryLabel
{
    void *ptr;
    const char *file;
    int line;
    MemoryLabel *prev;
    MemoryLabel *next;
};

static int INIT = 0;
static pthread_mutex_t MUTEX = {0};
static MemoryLabel *HEAD = NULL;
static MemoryLabel *TAIL = NULL;

int mutex_create_or_lock()
{
    if (INIT == 0 && pthread_mutex_init(&MUTEX, NULL) != 0) goto goto_mutex_create_or_lock_error;
    if (pthread_mutex_lock(&MUTEX) != 0) goto goto_mutex_create_or_lock_error;
    INIT = 1; // set after mutex locked!
    return 0;
goto_mutex_create_or_lock_error:
    _ram_throw_error(RAM_ERROR_MUTEX_FAILED);
    return 1;
}

int mutex_unlock()
{
    if (INIT == 1 && pthread_mutex_unlock(&MUTEX) != 0)
    {
        _ram_throw_error(RAM_ERROR_MUTEX_FAILED);
        return 1;
    }
    return 0;
}

int mutex_unlock_and_destroy()
{
    if (INIT == 1)
    {
        INIT = 0;
        if (pthread_mutex_unlock(&MUTEX) != 0 || pthread_mutex_destroy(&MUTEX) != 0)
        {
            _ram_throw_error(RAM_ERROR_MUTEX_FAILED);
            return 1;
        }
    }
    return 0;
}

#define DO_FN_WITH_MUTEX(fn, return_value) if(fn() != 0) return return_value;
#define CREATE_OR_LOCK_MUTEX(return_value) DO_FN_WITH_MUTEX(mutex_create_or_lock, return_value)
#define UNLOCK_MUTEX(return_value) DO_FN_WITH_MUTEX(mutex_unlock, return_value)
#define UNLOCK_AND_DESTROY_MUTEX(return_value) DO_FN_WITH_MUTEX(mutex_unlock_and_destroy, return_value)

MemoryLabel *create_memory_label_generic(void *ptr, const char *file, int line, MemoryLabel *prev, MemoryLabel *next)
{
    MemoryLabel *mem_label = malloc(sizeof(MemoryLabel));
    if (mem_label == NULL)
    {
        _ram_throw_error(RAM_ERROR_LABEL_ALLOCATION_FAILED);
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
    if (ptr == NULL) return 0;
    if (HEAD == NULL)
    {
        if ((HEAD = create_memory_label_generic(NULL, NULL, 0, NULL, NULL)) == NULL) return 1;
        TAIL = HEAD;
    }
    MemoryLabel *mem_label = create_memory_label_generic(ptr, file, line, TAIL, NULL);
    if (mem_label == NULL) return 1;
    TAIL->next = mem_label;
    TAIL = mem_label;
    return 0;
}

MemoryLabel *find_memory_label(void *ptr)
{
    if (HEAD == NULL) return NULL;
    MemoryLabel *current = HEAD->next;
    while (current)
    {
        if (current->ptr == ptr)
        {
            return current;
        }
        current = current->next;
    }
    _ram_throw_error(RAM_ERROR_LABEL_NOT_VALID);
    return NULL;
}

void *_ram_malloc(size_t size, const char *file, int line)
{
    void *ptr = malloc(size);
#ifndef NDEBUG
    CREATE_OR_LOCK_MUTEX(NULL);
    if (create_memory_label(ptr, file, line) != 0)
    {
        free(ptr);
        ptr = NULL;
    }
    UNLOCK_MUTEX(NULL);
#endif // NDEBUG
    return ptr;
}

void *_ram_calloc(size_t nmemb, size_t size, const char *file, int line)
{
    void *ptr = calloc(nmemb, size);
#ifndef NDEBUG
    CREATE_OR_LOCK_MUTEX(NULL);
    if (create_memory_label(ptr, file, line) != 0)
    {
        free(ptr);
        ptr = NULL;
    }
    UNLOCK_MUTEX(NULL);
#endif // NDEBUG
    return ptr;
}

void *_ram_realloc(void *ptr, size_t size)
{
    void *new_ptr = realloc(ptr, size);
#ifndef NDEBUG
    CREATE_OR_LOCK_MUTEX(NULL);
    MemoryLabel *mem_label = find_memory_label(ptr);
    if (mem_label == NULL)
    {
        free(new_ptr);
        new_ptr = NULL;
    }
    else
    {
        mem_label->ptr = new_ptr;
    }
    UNLOCK_MUTEX(NULL);
#endif // NDEBUG
    return new_ptr;
}

void _ram_free(void *ptr)
{
    free(ptr);
#ifndef NDEBUG
    if (ptr == NULL) return;
    CREATE_OR_LOCK_MUTEX();
    MemoryLabel *mem_label = find_memory_label(ptr);
    if (mem_label == NULL)
    {
        UNLOCK_MUTEX();
        return;
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
    free(mem_label);
    if (TAIL == HEAD)
    {
        free(HEAD);
        HEAD = NULL;
        TAIL = NULL;
        UNLOCK_AND_DESTROY_MUTEX();
    }
    else
    {
        UNLOCK_MUTEX();
    }
#endif // NDEBUG
}

void ram_trace(ram_trace_fn fn)
{
#ifndef NDEBUG
    if (fn == NULL) return;
    CREATE_OR_LOCK_MUTEX();
    if (HEAD == NULL)
    {
        UNLOCK_MUTEX();
        return;
    }
    MemoryLabel *current = HEAD->next;
    while (current)
    {
        fn(current->ptr, current->file, current->line);
        current = current->next;
    }
    UNLOCK_MUTEX();
#endif // NDEBUG
}
