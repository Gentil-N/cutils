#include "ctools.h"

#include <assert.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>

#ifndef CTOOLS_MALLOC
#define CTOOLS_MALLOC malloc
#endif

#ifndef CTOOLS_CALLOC
#define CTOOLS_CALLOC calloc
#endif

#ifndef CTOOLS_REALLOC
#define CTOOLS_REALLOC realloc
#endif

#ifndef CTOOLS_FREE
#define CTOOLS_FREE free
#endif

/*
*   LIST
*/

struct ListGeneric
{
    size_t size;
    void *data;
    size_t _capacity;
};

static const size_t LIST_INIT_CAPACITY = 1;

void grow_list_if_needed(struct ListGeneric *generic, size_t elem_byte_size)
{
    if (generic->_capacity < generic->size)
    {
        generic->_capacity *= 2;
        void *new_data = CTOOLS_REALLOC(generic->data, generic->_capacity * elem_byte_size);
        assert(trk_change_register(generic->data, new_data) == 0);
        generic->data = new_data;
    }
}

void reduce_list_if_needed(struct ListGeneric *generic, size_t elem_byte_size)
{
    if (generic->_capacity > generic->size * 2 && generic->_capacity > 1)
    {
        generic->_capacity /= 2;
        void *new_data = CTOOLS_REALLOC(generic->data, generic->_capacity * elem_byte_size);
        assert(trk_change_register(generic->data, new_data) == 0);
        generic->data = new_data;
    }
}

void _list_generic_create(void *list, size_t size, size_t elem_byte_size, const char *file, int line)
{
    struct ListGeneric *generic = (struct ListGeneric *)list;
    generic->size = size;
    generic->_capacity = ((size == 0) ? LIST_INIT_CAPACITY : size);
    generic->data = CTOOLS_MALLOC(generic->_capacity * elem_byte_size);
    assert(trk_register(generic->data, file, line) == 0);
}

void _list_generic_destroy(void *list)
{
    struct ListGeneric *generic = (struct ListGeneric *)list;
    assert(trk_unregister(generic->data) == 0);
    CTOOLS_FREE(generic->data);
}

void _list_generic_resize(void *list, size_t elem_byte_size, size_t new_size)
{
    struct ListGeneric *generic = (struct ListGeneric *)list;
    generic->size = new_size;
    grow_list_if_needed(list, elem_byte_size);
    reduce_list_if_needed(list, elem_byte_size);
}

void _list_generic_append(void *list, size_t elem_byte_size, const void *elem)
{
    struct ListGeneric *generic = (struct ListGeneric *)list;
    generic->size += 1;
    grow_list_if_needed(generic, elem_byte_size);
    memcpy(generic->data + elem_byte_size * (generic->size - 1), elem, elem_byte_size);
}

void _list_generic_remove(void *list, size_t elem_byte_size, size_t index)
{
    struct ListGeneric *generic = (struct ListGeneric *)list;
    if (generic->size < index + 1)
    {
        return;
    }
    generic->size -= 1;
    memcpy(generic->data + elem_byte_size * index, generic->data + elem_byte_size * (index + 1), elem_byte_size * (generic->size - index));
    reduce_list_if_needed(generic, elem_byte_size);
}

void _list_generic_insert(void *list, size_t elem_byte_size, size_t index, const void *elem)
{
    struct ListGeneric *generic = (struct ListGeneric *)list;
    if (index > generic->size)
    {
        return;
    }
    generic->size += 1;
    grow_list_if_needed(generic, elem_byte_size);
    memcpy(generic->data + elem_byte_size * (index + 1), generic->data + elem_byte_size * index, elem_byte_size * (generic->size - 1 - index));
    memcpy(generic->data + elem_byte_size * index, elem, elem_byte_size);
}

size_t _list_generic_find(void *list, size_t elem_byte_size, const void *elem)
{
    struct ListGeneric *generic = (struct ListGeneric *)list;
    for (size_t i = 0; i < generic->size; ++i)
    {
        if (memcmp((void*)((size_t)generic->data + i * elem_byte_size), elem, elem_byte_size) == 0) return i;
    }
    return SIZE_MAX;
}

/*
*   RAM
*/

#include <stdio.h>

void *_ram_malloc(size_t size, const char *file, int line)
{
    void *ptr = CTOOLS_MALLOC(size);
#ifndef NDEBUG
    assert(trk_register(ptr, file, line) == 0);
#endif
    return ptr;
}

void *_ram_calloc(size_t nmemb, size_t size, const char *file, int line)
{
    void *ptr = CTOOLS_CALLOC(nmemb, size);
#ifndef NDEBUG
    assert(trk_register(ptr, file, line) == 0);
#endif
    return ptr;
}

void *_ram_realloc(void *ptr, size_t size)
{
    void *new_ptr = CTOOLS_REALLOC(ptr, size);
#ifndef NDEBUG
    assert(trk_change_register(ptr, new_ptr) == 0);
#endif
    return new_ptr;
}

void _ram_free(void *ptr)
{
    CTOOLS_FREE(ptr);
#ifndef NDEBUG
    assert(trk_unregister(ptr) == 0);
#endif
}

/*
*   TRACKER
*/

struct MemoryLabel
{
    void *ptr;
    const char *file;
    int line;
    struct MemoryLabel *prev;
    struct MemoryLabel *next;
};

static int TRK_INIT = 0;
static pthread_mutex_t TRK_MUTEX = {0};
static struct MemoryLabel *TRK_HEAD = NULL;
static struct MemoryLabel *TRK_TAIL = NULL;

int mutex_create_or_lock()
{
    if (TRK_INIT == 0 && pthread_mutex_init(&TRK_MUTEX, NULL) != 0) return 1;
    if (pthread_mutex_lock(&TRK_MUTEX) != 0) return 1;
    TRK_INIT = 1; // set after mutex locked!
    return 0;
}

int mutex_unlock()
{
    if (TRK_INIT == 1 && pthread_mutex_unlock(&TRK_MUTEX) != 0) return 1;
    return 0;
}

int mutex_unlock_and_destroy()
{
    if (TRK_INIT == 1)
    {
        TRK_INIT = 0;// assuming that MUTEX is already locked
        if (pthread_mutex_unlock(&TRK_MUTEX) != 0 || pthread_mutex_destroy(&TRK_MUTEX) != 0) return 1;
    }
    return 0;
}

struct MemoryLabel *create_memory_label_generic(void *ptr, const char *file, int line, struct MemoryLabel *prev, struct MemoryLabel *next)
{
    struct MemoryLabel *mem_label = CTOOLS_MALLOC(sizeof(struct MemoryLabel));
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
    if (TRK_HEAD == NULL)
    {
        if ((TRK_HEAD = create_memory_label_generic(NULL, NULL, 0, NULL, NULL)) == NULL) return 1;
        TRK_TAIL = TRK_HEAD;
    }
    struct MemoryLabel *mem_label = create_memory_label_generic(ptr, file, line, TRK_TAIL, NULL);
    if (mem_label == NULL) return 1;
    TRK_TAIL->next = mem_label;
    TRK_TAIL = mem_label;
    return 0;
}

struct MemoryLabel *find_memory_label(void *ptr)
{
    if (TRK_HEAD == NULL) return NULL;
    struct MemoryLabel *current = TRK_HEAD->next;
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

int trk_register(void *address, const char *file, int line)
{
    if (address == NULL) return 0;
    if (mutex_create_or_lock() != 0) return 1;
    if (create_memory_label(address, file, line) != 0) return 3;
    if (mutex_unlock() != 0) return 1;
    return 0;
}

int trk_unregister(void *address)
{
    if (address == NULL) return 0;
    if (mutex_create_or_lock() != 0) return 1;
    struct MemoryLabel *mem_label;
    if ((mem_label = find_memory_label(address)) == NULL)
    {
        if (mutex_unlock() != 0) return 1;
        return 2;
    }
    mem_label->prev->next = mem_label->next;
    if (mem_label->next)
    {
        mem_label->next->prev = mem_label->prev;
    }
    else
    {
        TRK_TAIL = mem_label->prev;
    }
    CTOOLS_FREE(mem_label);
    if (TRK_TAIL == TRK_HEAD)
    {
        CTOOLS_FREE(TRK_HEAD);
        TRK_HEAD = NULL;
        TRK_TAIL = NULL;
        if (mutex_unlock_and_destroy() != 0) return 1;
    }
    else
    {
        if (mutex_unlock() != 0) return 1;
    }
    return 0;
}

int trk_change_register(void *old_address, void *new_address)
{
    if (old_address == NULL) return 0;
    if (mutex_create_or_lock() != 0) return 1;
    struct MemoryLabel *mem_label;
    if ((mem_label = find_memory_label(old_address)) == NULL)
    {
        if (mutex_unlock() != 0) return 1;
        return 2;
    }
    mem_label->ptr = new_address;
    if (mutex_unlock() != 0) return 1;
    return 0;
}

int trk_trace(trk_trace_fn fn)
{
    if (fn == NULL) return 0;
    if (mutex_create_or_lock() != 0) return 1;
    if (TRK_HEAD == NULL)
    {
        if (mutex_unlock() != 0) return 1;
        return 0;
    }
    struct MemoryLabel *current = TRK_HEAD->next;
    while (current)
    {
        fn(current->ptr, current->file, current->line);
        current = current->next;
    }
    if (mutex_unlock() != 0) return 1;
    return 0;
}
