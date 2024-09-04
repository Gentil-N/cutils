#ifndef __LIST_H__
#define __LIST_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

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

#ifdef __cplusplus
}
#endif

#endif // __LIST_H__
