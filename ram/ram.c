#include "ram.h"

#include <stdlib.h>
#include "../tracker/trk.h"
#include <assert.h>

#ifndef RAM_MALLOC
#define RAM_MALLOC malloc
#endif

#ifndef RAM_CALLOC
#define RAM_CALLOC calloc
#endif

#ifndef RAM_REALLOC
#define RAM_REALLOC realloc
#endif

#ifndef RAM_FREE
#define RAM_FREE free
#endif

void *_ram_malloc(size_t size, const char *file, int line)
{
    void *ptr = RAM_MALLOC(size);
    assert(trk_register(ptr, file, line) == TRK_RESULT_OK);
    return ptr;
}

void *_ram_calloc(size_t nmemb, size_t size, const char *file, int line)
{
    void *ptr = RAM_CALLOC(nmemb, size);
    assert(trk_register(ptr, file, line) == TRK_RESULT_OK);
    return ptr;
}

void *_ram_realloc(void *ptr, size_t size)
{
    void *new_ptr = RAM_REALLOC(ptr, size);
    assert(trk_change_register(ptr, new_ptr) == TRK_RESULT_OK);
    return new_ptr;
}

void _ram_free(void *ptr)
{
    RAM_FREE(ptr);
    assert(trk_unregister(ptr) == TRK_RESULT_OK);
}
