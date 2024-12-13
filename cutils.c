#include "cutils.h"

#include <assert.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef CUTILS_MALLOC
#define CUTILS_MALLOC malloc
#endif

#ifndef CUTILS_CALLOC
#define CUTILS_CALLOC calloc
#endif

#ifndef CUTILS_REALLOC
#define CUTILS_REALLOC realloc
#endif

#ifndef CUTILS_FREE
#define CUTILS_FREE free
#endif

/*
 *   LOGGER
 */

static int LOG_INIT = 0;
static pthread_mutex_t LOG_MUTEX = {0};
static log_callback_fn LOG_CALLBACK = NULL;

static void std_callback(int level, int code, const char *file, int line, const char *msg)
{
    switch (level)
    {
    case LOG_LEVEL_INFO:
        fprintf(stdout, "[INFO](%s: %i) %s\n", filename(file), line, msg);
        fflush(stdout);
        break;
    case LOG_LEVEL_DEBUG:
#ifndef NDEBUG
        fprintf(stdout, "\033[0;32m[DEBUG](%s: %i) %s\n", filename(file), line, msg);
        fflush(stdout);
#endif // NDEBUG
        break;
    case LOG_LEVEL_WARN:
        fprintf(stdout, "\033[0;33m[WARN](%s: %i) %s\n", filename(file), line, msg);
        fflush(stdout);
        break;
    case LOG_LEVEL_ERROR:
        fprintf(stderr, "\033[0;31m[ERROR](%s: %i) %s\n", filename(file), line, msg);
        break;
    case LOG_LEVEL_FATAL:
        fprintf(stderr, "\033[0;35m[FATAL](%s: %i) %s\naborting...\n", filename(file), line, msg);
        break;
    }
}

int log_set_callback(log_callback_fn fn)
{
    if (fn == NULL && LOG_INIT == 1)
    {
        LOG_INIT = 0;
        LOG_CALLBACK = NULL;
        if (pthread_mutex_destroy(&(LOG_MUTEX)) != 0)
            return 1;
    }
    else if (LOG_INIT == 0)
    {
        LOG_INIT = 1;
        LOG_CALLBACK = fn;
        if (pthread_mutex_init(&(LOG_MUTEX), NULL) != 0)
            return 1;
    }
    return 0;
}

int _log(int level, int code, const char *file, int line, const char *fmt, ...)
{
    if (LOG_CALLBACK == NULL)
    {
        if (log_set_callback(std_callback) != 0)
            return 1;
    }
    if (LOG_INIT == 1 && LOG_CALLBACK != NULL)
    {
        if (pthread_mutex_lock(&(LOG_MUTEX)) != 0)
            return 1;
        va_list ap, ap_copy;
        va_start(ap, fmt);
        va_copy(ap_copy, ap);
        size_t len = vsnprintf(0, 0, fmt, ap_copy);
        char *msg = malloc(len + 1);
        vsnprintf(msg, len + 1, fmt, ap);
        (LOG_CALLBACK)(level, code, file, line, msg);
        free(msg);
        va_end(ap);
        if (pthread_mutex_unlock(&(LOG_MUTEX)) != 0)
            return 1;
    }
    return 0;
}

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
        void *new_data = _mem_realloc(generic->data, generic->_capacity * elem_byte_size);
        generic->data = new_data;
    }
}

void reduce_list_if_needed(struct ListGeneric *generic, size_t elem_byte_size)
{
    if (generic->_capacity > generic->size * 2 && generic->_capacity > 1)
    {
        generic->_capacity /= 2;
        void *new_data = _mem_realloc(generic->data, generic->_capacity * elem_byte_size);
        generic->data = new_data;
    }
}

void _list_generic_create(void *list, size_t size, size_t elem_byte_size, const char *file, int line)
{
    struct ListGeneric *generic = (struct ListGeneric *)list;
    generic->size = size;
    generic->_capacity = ((size == 0) ? LIST_INIT_CAPACITY : size);
    generic->data = _mem_malloc(generic->_capacity * elem_byte_size, file, line);
}

void _list_generic_destroy(void *list)
{
    struct ListGeneric *generic = (struct ListGeneric *)list;
    _mem_free(generic->data);
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
    memcpy(generic->data + elem_byte_size * index, generic->data + elem_byte_size * (index + 1),
           elem_byte_size * (generic->size - index));
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
    memcpy(generic->data + elem_byte_size * (index + 1), generic->data + elem_byte_size * index,
           elem_byte_size * (generic->size - 1 - index));
    memcpy(generic->data + elem_byte_size * index, elem, elem_byte_size);
}

size_t _list_generic_find(void *list, size_t elem_byte_size, const void *elem)
{
    struct ListGeneric *generic = (struct ListGeneric *)list;
    for (size_t i = 0; i < generic->size; ++i)
    {
        if (memcmp((void *)((size_t)generic->data + i * elem_byte_size), elem, elem_byte_size) == 0)
            return i;
    }
    return SIZE_MAX;
}

/*
 *   MEM
 */

#include <stdio.h>

void *_mem_malloc(size_t size, const char *file, int line)
{
    void *ptr = CUTILS_MALLOC(size);
#ifndef NDEBUG
    assert(tracker_register(ptr, file, line) == 0);
#endif
    return ptr;
}

void *_mem_calloc(size_t nmemb, size_t size, const char *file, int line)
{
    void *ptr = CUTILS_CALLOC(nmemb, size);
#ifndef NDEBUG
    assert(tracker_register(ptr, file, line) == 0);
#endif
    return ptr;
}

void *_mem_realloc(void *ptr, size_t size)
{
    void *new_ptr = CUTILS_REALLOC(ptr, size);
#ifndef NDEBUG
    assert(tracker_change_register(ptr, new_ptr) == 0);
#endif
    return new_ptr;
}

void _mem_free(void *ptr)
{
    CUTILS_FREE(ptr);
#ifndef NDEBUG
    assert(tracker_unregister(ptr) == 0);
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

static int TRACKER_INIT = 0;
static pthread_mutex_t TRACKER_MUTEX = {0};
static struct MemoryLabel *TRACKER_HEAD = NULL;
static struct MemoryLabel *TRACKER_TAIL = NULL;

int mutex_create_or_lock()
{
    if (TRACKER_INIT == 0 && pthread_mutex_init(&TRACKER_MUTEX, NULL) != 0)
        return 1;
    if (pthread_mutex_lock(&TRACKER_MUTEX) != 0)
        return 1;
    TRACKER_INIT = 1; // set after mutex locked!
    return 0;
}

int mutex_unlock()
{
    if (TRACKER_INIT == 1 && pthread_mutex_unlock(&TRACKER_MUTEX) != 0)
        return 1;
    return 0;
}

int mutex_unlock_and_destroy()
{
    if (TRACKER_INIT == 1)
    {
        TRACKER_INIT = 0; // assuming that MUTEX is already locked
        if (pthread_mutex_unlock(&TRACKER_MUTEX) != 0 || pthread_mutex_destroy(&TRACKER_MUTEX) != 0)
            return 1;
    }
    return 0;
}

struct MemoryLabel *create_memory_label_generic(void *ptr, const char *file, int line, struct MemoryLabel *prev,
                                                struct MemoryLabel *next)
{
    struct MemoryLabel *mem_label = CUTILS_MALLOC(sizeof(struct MemoryLabel));
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
    if (TRACKER_HEAD == NULL)
    {
        if ((TRACKER_HEAD = create_memory_label_generic(NULL, NULL, 0, NULL, NULL)) == NULL)
            return 1;
        TRACKER_TAIL = TRACKER_HEAD;
    }
    struct MemoryLabel *mem_label = create_memory_label_generic(ptr, file, line, TRACKER_TAIL, NULL);
    if (mem_label == NULL)
        return 1;
    TRACKER_TAIL->next = mem_label;
    TRACKER_TAIL = mem_label;
    return 0;
}

struct MemoryLabel *find_memory_label(void *ptr)
{
    if (TRACKER_HEAD == NULL)
        return NULL;
    struct MemoryLabel *current = TRACKER_HEAD->next;
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

int tracker_register(void *address, const char *file, int line)
{
    if (address == NULL)
        return 0;
    if (mutex_create_or_lock() != 0)
        return 1;
    if (create_memory_label(address, file, line) != 0)
        return 3;
    if (mutex_unlock() != 0)
        return 1;
    return 0;
}

int tracker_unregister(void *address)
{
    if (address == NULL)
        return 0;
    if (mutex_create_or_lock() != 0)
        return 1;
    struct MemoryLabel *mem_label;
    if ((mem_label = find_memory_label(address)) == NULL)
    {
        if (mutex_unlock() != 0)
            return 1;
        return 2;
    }
    mem_label->prev->next = mem_label->next;
    if (mem_label->next)
    {
        mem_label->next->prev = mem_label->prev;
    }
    else
    {
        TRACKER_TAIL = mem_label->prev;
    }
    CUTILS_FREE(mem_label);
    if (TRACKER_TAIL == TRACKER_HEAD)
    {
        CUTILS_FREE(TRACKER_HEAD);
        TRACKER_HEAD = NULL;
        TRACKER_TAIL = NULL;
        if (mutex_unlock_and_destroy() != 0)
            return 1;
    }
    else
    {
        if (mutex_unlock() != 0)
            return 1;
    }
    return 0;
}

int tracker_change_register(void *old_address, void *new_address)
{
    if (old_address == NULL)
        return 0;
    if (mutex_create_or_lock() != 0)
        return 1;
    struct MemoryLabel *mem_label;
    if ((mem_label = find_memory_label(old_address)) == NULL)
    {
        if (mutex_unlock() != 0)
            return 1;
        return 2;
    }
    mem_label->ptr = new_address;
    if (mutex_unlock() != 0)
        return 1;
    return 0;
}

int tracker_trace(tracker_trace_fn fn)
{
    if (fn == NULL)
        return 0;
    if (mutex_create_or_lock() != 0)
        return 1;
    if (TRACKER_HEAD == NULL)
    {
        if (mutex_unlock() != 0)
            return 1;
        return 0;
    }
    struct MemoryLabel *current = TRACKER_HEAD->next;
    while (current)
    {
        fn(current->ptr, current->file, current->line);
        current = current->next;
    }
    if (mutex_unlock() != 0)
        return 1;
    return 0;
}
