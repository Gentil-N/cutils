#ifndef __RAM_H__
#define __RAM_H__

#include <stddef.h>
#include "../err/err.h"

#ifdef __cplusplus
extern "C"
{
#endif

void *_ram_malloc(size_t size, const char *file, int line);
void *_ram_calloc(size_t nmemb, size_t size, const char *file, int line);
void *_ram_realloc(void *ptr, size_t size);
void _ram_free(void *ptr);

typedef enum RamErrorCode
{
    RAM_ERROR_NOPE = 0,
    RAM_ERROR_LABEL_ALLOCATION_FAILED = 1,
    RAM_ERROR_MUTEX_FAILED = 2,
    RAM_ERROR_LABEL_NOT_VALID = 3,
} RamErrorCode;

ERR_DEFINE(ram);

typedef void (*ram_trace_fn)(void *ptr, const char *file, int line);

void ram_trace(ram_trace_fn fn);

#define ram_malloc(size) _ram_malloc(size, __FILE__, __LINE__)
#define ram_calloc(nmemb, size) _ram_calloc(nmemb, size, __FILE__, __LINE__)
#define ram_realloc(ptr, size) _ram_realloc(ptr, size)
#define ram_free(ptr) _ram_free(ptr)
#define ram_alloc(type) ram_malloc(sizeof(type))
#define ram_alloc_init(type, name) type *name = ram_malloc(sizeof(type))

#ifdef __cplusplus
}
#endif

#endif //__RAM_H__
