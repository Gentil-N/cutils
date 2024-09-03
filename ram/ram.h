#ifndef __RAM_H__
#define __RAM_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

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

#ifdef __cplusplus
}
#endif

#endif //__RAM_H__
