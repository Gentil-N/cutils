#ifndef __CUTILS_BASE_H__
#define __CUTILS_BASE_H__

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*
 *   MISCELLANEOUS
 */
#define for_loop(index_name, limit) for (size_t index_name = 0; index_name < limit; ++index_name)
#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define clamp(x, low, up) min((up), max((x), (low)))

/*
 *   STRING
 */
#define filename(path) strrchr(path, '/') ? strrchr(path, '/') + 1 : path

    /*
     *   LOGGER
     */

    enum LogLevel
    {
        LOG_LEVEL_INFO = 1,
        LOG_LEVEL_DEBUG = 2,
        LOG_LEVEL_WARN = 3,
        LOG_LEVEL_ERROR = 4,
        LOG_LEVEL_FATAL = 5
    };

    typedef void (*log_callback_fn)(int level, int code, const char *file, int line, const char *msg);
    int log_set_callback(log_callback_fn fn);
    int _log(int level, int code, const char *file, int line, const char *fmt, ...);
#define log_info(...) _log(LOG_LEVEL_INFO, -1, __FILE__, __LINE__, __VA_ARGS__)
#ifndef NDEBUG
#define log_debug(...) _log(LOG_LEVEL_DEBUG, -1, __FILE__, __LINE__, __VA_ARGS__)
#else
#define log_debug(...)
#endif // NDEBUG
#define log_warn(...) _log(LOG_LEVEL_WARN, -1, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) _log(LOG_LEVEL_ERROR, -1, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...)                                                                                                 \
    {                                                                                                                  \
        _log(LOG_LEVEL_FATAL, -1, __FILE__, __LINE__, __VA_ARGS__);                                                    \
        exit(1);                                                                                                       \
    }
    /*
     *   LIST
     */

    void _list_generic_create(void *list, size_t size, size_t elem_byte_size, const char *file, int line);
    void _list_generic_destroy(void *list);
    void _list_generic_resize(void *list, size_t elem_byte_size, size_t new_size);
    void _list_generic_append(void *list, size_t elem_byte_size, const void *elem);
    void _list_generic_remove(void *list, size_t elem_byte_size, size_t index);
    void _list_generic_insert(void *list, size_t elem_byte_size, size_t index, const void *elem);
    size_t _list_generic_find(void *list, size_t elem_byte_size, const void *elem);

#define LIST(type)                                                                                                     \
    struct List_##type                                                                                                 \
    {                                                                                                                  \
        size_t size;                                                                                                   \
        type *data;                                                                                                    \
        size_t _capacity;                                                                                              \
    };                                                                                                                 \
    static inline struct List_##type _list_##type##_create(size_t size, const char *file, int line)                    \
    {                                                                                                                  \
        struct List_##type list = {0};                                                                                 \
        _list_generic_create(&list, size, sizeof(type), file, line);                                                   \
        return list;                                                                                                   \
    }                                                                                                                  \
    static inline void list_##type##_destroy(struct List_##type *list) { _list_generic_destroy(list); }                \
    static inline void list_##type##_resize(struct List_##type *list, size_t new_size)                                 \
    {                                                                                                                  \
        _list_generic_resize(list, sizeof(type), new_size);                                                            \
    }                                                                                                                  \
    static inline void list_##type##_append(struct List_##type *list, const type *elem)                                \
    {                                                                                                                  \
        _list_generic_append(list, sizeof(type), elem);                                                                \
    }                                                                                                                  \
    static inline void list_##type##_remove(struct List_##type *list, size_t index)                                    \
    {                                                                                                                  \
        _list_generic_remove(list, sizeof(type), index);                                                               \
    }                                                                                                                  \
    static inline void list_##type##_insert(struct List_##type *list, size_t index, const type *elem)                  \
    {                                                                                                                  \
        _list_generic_insert(list, sizeof(type), index, elem);                                                         \
    }                                                                                                                  \
    static inline size_t list_##type##_find(struct List_##type *list, const type *elem)                                \
    {                                                                                                                  \
        return _list_generic_find(list, sizeof(type), elem);                                                           \
    }                                                                                                                  \
    static inline size_t list_##type##_remove_on_finding(struct List_##type *list, const type *elem)                   \
    {                                                                                                                  \
        size_t index = _list_generic_find(list, sizeof(type), elem);                                                   \
        if (index != SIZE_MAX)                                                                                         \
            _list_generic_remove(list, sizeof(type), index);                                                           \
        return index;                                                                                                  \
    }

#define list_create(type, size) _list_##type##_create(size, __FILE__, __LINE__)
#define for_list(index_name, list) for (size_t index_name = 0; index_name < list.size; ++index_name)

    /*
     *   MEM
     */

    void *_mem_malloc(size_t size, const char *file, int line);
    void *_mem_calloc(size_t nmemb, size_t size, const char *file, int line);
    void *_mem_realloc(void *ptr, size_t size);
    void _mem_free(void *ptr);

#define mem_malloc(size) _mem_malloc(size, __FILE__, __LINE__)
#define mem_calloc(nmemb, size) _mem_calloc(nmemb, size, __FILE__, __LINE__)
#define mem_realloc(ptr, size) _mem_realloc(ptr, size)
#define mem_free(ptr) _mem_free(ptr)
#define mem_alloc(type) mem_malloc(sizeof(type))
#define mem_alloc_init(type, name) type *name = mem_malloc(sizeof(type))

    /*
     *   TRACKER
     */

    typedef void (*tracker_trace_fn)(void *address, const char *file, int line);

    int tracker_register(void *address, const char *file, int line);
    int tracker_unregister(void *address);
    int tracker_change_register(void *old_address, void *new_address);
    int tracker_trace(tracker_trace_fn fn);

#ifdef __cplusplus
}
#endif

#endif // __CUTILS_BASE_H__
