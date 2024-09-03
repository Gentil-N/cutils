#include "list.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../tracker/trk.h"

#ifndef LIST_MALLOC
#define LIST_MALLOC malloc
#endif

#ifndef LIST_REALLOC
#define LIST_REALLOC realloc
#endif

#ifndef LIST_FREE
#define LIST_FREE free
#endif

struct ListGeneric
{
    size_t size;
    size_t _capacity;
    void *data;
};

static const size_t INIT_CAPACITY = 1;

#define meta_to_list(list) (void *)(&(list[1]));
#define list_to_meta(list) (&((struct ListGeneric *)(list))[-1])

void grow_list_if_needed(struct ListGeneric *generic, size_t elem_byte_size)
{
    if (generic->_capacity < generic->size)
    {
        generic->_capacity *= 2;
        void *new_data = LIST_REALLOC(generic->data, generic->_capacity * elem_byte_size);
        assert(trk_change_register(generic->data, new_data) == TRK_RESULT_OK);
        generic->data = new_data;
    }
}

void reduce_list_if_needed(struct ListGeneric *generic, size_t elem_byte_size)
{
    if (generic->_capacity > generic->size * 2 && generic->_capacity > 1)
    {
        generic->_capacity /= 2;
        void *new_data = LIST_REALLOC(generic->data, generic->_capacity * elem_byte_size);
        assert(trk_change_register(generic->data, new_data) == TRK_RESULT_OK);
        generic->data = new_data;
    }
}

void _list_generic_create(void *list, size_t size, size_t elem_byte_size, const char *file, int line)
{
    struct ListGeneric *generic = (struct ListGeneric *)list;
    generic->size = size;
    generic->_capacity = ((size == 0) ? INIT_CAPACITY : size);
    generic->data = LIST_MALLOC(generic->_capacity * elem_byte_size);
    assert(trk_register(generic->data, file, line) == TRK_RESULT_OK);
}

void _list_generic_destroy(void *list)
{
    struct ListGeneric *generic = (struct ListGeneric *)list;
    assert(trk_unregister(generic->data) == TRK_RESULT_OK);
    LIST_FREE(generic->data);
}

void _list_generic_resize(void *list, size_t elem_byte_size, size_t new_size)
{
    struct ListGeneric *generic = (struct ListGeneric *)list;
    generic->size = new_size;
    grow_list_if_needed(list, elem_byte_size);
    reduce_list_if_needed(list, elem_byte_size);
}

void _list_generic_append(void *list, size_t elem_byte_size, void *elem)
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

void _list_generic_insert(void *list, size_t elem_byte_size, size_t index, void *elem)
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
