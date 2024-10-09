#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include "../cutils.h"

LOG_DEFINE(main, error)

#include <pthread.h>
#include <stdarg.h>
LOG_DECLARE(main, error)

void trace(void *ptr, const char *file, int line)
{
    printf("TRACE: (line) %i | (file) %s | (ptr) %p\n", line, file, ptr);
}

LIST(int);
typedef struct List_int List_int;
LIST(List_int);

void parse_data(size_t size, const float *data)
{
    for_loop(i, size)
    {
        printf("%f\n", data[i]);
    }
}

int main()
{
    Array(float, array, 4, {1.0f, 2.0f, 3.0f, 4.0f});
    //array.size = 2; // Error! Prevent unexcpected resize of the array
    array.data[0] = 0.0f;
    array.data[3] = 5.0f;
    parse_data(array.size, array.data);

    struct List_int list_a = list_create(int, 10);
    struct List_int list_b = list_create(int, 10);
    struct List_List_int list_c = list_create(List_int, 10);

    assert(tracker_trace(trace) == 0);

    int a = 1;
    list_int_append(&list_a, &a);
    list_a.data[0] = 2;
    printf("%lu %i\n", list_a.size, list_a.data[10]);
    list_List_int_append(&list_c, &list_a);
    printf("%lu %lu\n", list_c.size, list_c.data[10].size);

    list_List_int_destroy(&list_c);
    list_int_destroy(&list_b);
    list_int_destroy(&list_a);

    assert(tracker_trace(trace) == 0);

    return 0;
}
