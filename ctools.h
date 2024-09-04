#ifndef __CTOOLS_H__
#define __CTOOLS_H__

#include <stdlib.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*
*   LOGGER
*/

#define LOG_DEFINE(suffix, type)                                                                                    \
typedef void (*suffix##_##type##_callback_fn)(int level, int code, const char *file, int line, const char *msg);    \
int suffix##_set_##type##_callback(suffix##_##type##_callback_fn fn);                                               \
int _##suffix##_throw_##type(int level, int code, const char *file, int line, const char *fmt, ...);                \

#define LOG_DECLARE(suffix, type)                                                                       \
static int suffix##_##type##_INIT = 0;                                                                  \
static pthread_mutex_t suffix##_##type##_MUTEX = {0};                                                   \
static suffix##_##type##_callback_fn suffix##_##type##_CALLBACK = NULL;                                 \
int suffix##_set_##type##_callback(suffix##_##type##_callback_fn fn)                                    \
{                                                                                                       \
    if (fn == NULL && suffix##_##type##_INIT == 1)                                                      \
    {                                                                                                   \
        suffix##_##type##_INIT = 0;                                                                     \
        suffix##_##type##_CALLBACK = NULL;                                                              \
        if (pthread_mutex_destroy(&(suffix##_##type##_MUTEX)) != 0) return 1;                           \
    }                                                                                                   \
    else if (suffix##_##type##_INIT == 0)                                                               \
    {                                                                                                   \
        suffix##_##type##_INIT = 1;                                                                     \
        suffix##_##type##_CALLBACK = fn;                                                                \
        if (pthread_mutex_init(&(suffix##_##type##_MUTEX), NULL) != 0) return 1;                        \
    }                                                                                                   \
    return 0;                                                                                           \
}                                                                                                       \
int _##suffix##_throw_##type(int level, int code, const char *file, int line, const char *fmt, ...)     \
{                                                                                                       \
    if(suffix##_##type##_INIT == 1 && suffix##_##type##_CALLBACK != NULL)                               \
    {                                                                                                   \
        if (pthread_mutex_lock(&(suffix##_##type##_MUTEX)) != 0) return 1;                              \
        va_list ap, ap_copy;                                                                            \
        va_start(ap, fmt);                                                                              \
        va_copy(ap_copy, ap);                                                                           \
        size_t len = vsnprintf(0, 0, fmt, ap_copy);                                                     \
        char *msg = malloc(len + 1);                                                                    \
        vsnprintf(msg, len + 1, fmt, ap);                                                               \
        (suffix##_##type##_CALLBACK)(level, code, file, line, msg);                                     \
        free(msg);                                                                                      \
        va_end(ap);                                                                                     \
        if (pthread_mutex_unlock(&(suffix##_##type##_MUTEX)) != 0) return 1;                            \
    }                                                                                                   \
    return 0;                                                                                           \
}                                                                                                       \

/*
*   LIST
*/

void _list_generic_create(void *list, size_t size, size_t elem_byte_size, const char *file, int line);
void _list_generic_destroy(void *list);
void _list_generic_resize(void *list, size_t elem_byte_size, size_t new_size);
void _list_generic_append(void *list, size_t elem_byte_size, void *elem);
void _list_generic_remove(void *list, size_t elem_byte_size, size_t index);
void _list_generic_insert(void *list, size_t elem_byte_size, size_t index, void *elem);

#define LIST(type)                                                                                  \
struct List_##type                                                                                  \
{                                                                                                   \
    const size_t size;                                                                              \
    type *data;                                                                                     \
};                                                                                                  \
struct List_##type _list_##type##_create(size_t size, const char *file, int line)                   \
{                                                                                                   \
    struct List_##type list = {0};                                                                  \
    _list_generic_create(&list, size, sizeof(type), file, line);                                    \
    return list;                                                                                    \
}                                                                                                   \
void list_##type##_destroy(struct List_##type *list)                                                \
{                                                                                                   \
    _list_generic_destroy(list);                                                                    \
}                                                                                                   \
void list_##type##_resize(struct List_##type *list, size_t new_size)                                \
{                                                                                                   \
    _list_generic_resize(list, sizeof(type), new_size);                                             \
}                                                                                                   \
void list_##type##_append(struct List_##type *list, type *elem)                                     \
{                                                                                                   \
    _list_generic_append(list, sizeof(type), elem);                                                 \
}                                                                                                   \
void list_##type##_remove(struct List_##type *list, size_t index)                                   \
{                                                                                                   \
    _list_generic_remove(list, sizeof(type), index);                                                \
}                                                                                                   \
void list_##type##_insert(struct List_##type *list, size_t index, type *elem)                       \
{                                                                                                   \
    _list_generic_insert(list, sizeof(type), index, elem);                                          \
}                                                                                                   \

#define list_create(type, size) _list_##type##_create(size, __FILE__, __LINE__)

/*
*   RAM
*/

void *_ram_malloc(size_t size, const char *file, int line);
void *_ram_calloc(size_t nmemb, size_t size, const char *file, int line);
void *_ram_realloc(void *ptr, size_t size);
void _ram_free(void *ptr);

#define ram_malloc(size) _ram_malloc(size, __FILE__, __LINE__)
#define ram_calloc(nmemb, size) _ram_calloc(nmemb, size, __FILE__, __LINE__)
#define ram_realloc(ptr, size) _ram_realloc(ptr, size)
#define ram_free(ptr) _ram_free(ptr)
#define ram_alloc(type) ram_malloc(sizeof(type))
#define ram_alloc_init(type, name) type *name = ram_malloc(sizeof(type))

/*
*   TRACKER
*/

enum TrkResult
{
    TRK_RESULT_OK = 0,
    TRK_RESULT_ERROR_ADDRESS_NOT_FOUND = 1,
    TRK_RESULT_ERROR_OUT_OF_MEMORY = 2,
    TRK_RESULT_ERROR_INTERNAL_PTHREAD = 3,
};

typedef void (*trk_trace_fn)(void *address, const char *file, int line);

enum TrkResult _trk_register(void *address, const char *file, int line);
enum TrkResult _trk_unregister(void *address);
enum TrkResult _trk_change_register(void *old_address, void *new_address);
enum TrkResult _trk_trace(trk_trace_fn fn);

#ifndef NDEBUG
#define trk_register(address, file, line) _trk_register(address, file, line)
#define trk_unregister(address) _trk_unregister(address)
#define trk_change_register(old_address, new_address) _trk_change_register(old_address, new_address)
#define trk_trace(fn) _trk_trace(fn)
#else
#define trk_register(address, file, line)
#define trk_unregister(address)
#define trk_change_register(old_address, new_address)
#define trk_trace(fn)
#endif // NDEBUG

#ifdef __cplusplus
}
#endif

#endif // __CTOOLS_H__