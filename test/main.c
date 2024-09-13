#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include "../ctools.h"

/*void error_callback(int level, int code, const char *file, int line, const char *msg)
{
    fprintf(stderr, "ERROR (level) %i | (code) %i | (file) %s | (line) %i | (msg) %s", level, code, file, line, msg);
    fprintf(stderr, "\n");
    fflush(stderr);
}

LOG_DEFINE(main, error)

#include <pthread.h>
#include <stdarg.h>
LOG_DECLARE(main, error)*/

void trace(void *ptr, const char *file, int line)
{
    printf("TRACE: (line) %i | (file) %s | (ptr) %p\n", line, file, ptr);
}

LIST(int)

typedef struct List_int List_int;

LIST(List_int)

int main()
{
    /*int *a = malloc(sizeof(int));
    assert(trk_register(a, __FILE__, __LINE__) == TRK_RESULT_OK);
    assert(trk_trace(trace) == TRK_RESULT_OK);
    printf("haha\n");
    int *b = malloc(sizeof(int));
    assert(trk_register(b, __FILE__, __LINE__) == TRK_RESULT_OK);
    assert(trk_trace(trace) == TRK_RESULT_OK);
    printf("haha\n");
    int *c = malloc(sizeof(int));
    assert(trk_register(c, __FILE__, __LINE__) == TRK_RESULT_OK);
    assert(trk_trace(trace) == TRK_RESULT_OK);
    printf("haha\n");
    assert(trk_unregister(a) == TRK_RESULT_OK);
    free(a);
    assert(trk_trace(trace) == TRK_RESULT_OK);
    printf("haha\n");
    assert(trk_unregister(c) == TRK_RESULT_OK);
    free(c);
    assert(trk_trace(trace) == TRK_RESULT_OK);
    printf("haha\n");
    assert(trk_unregister(b) == TRK_RESULT_OK);
    free(b);
    assert(trk_trace(trace) == TRK_RESULT_OK);
    printf("haha\n");*/

    /*main_set_error_callback(error_callback);
    _main_throw_error(0, 0, __FILE__, __LINE__, "%s %i", "amount =", 1);
    main_set_error_callback(NULL);*/

    /*int *a = ram_malloc(sizeof(int));
    assert(trk_trace(trace) == TRK_RESULT_OK);
    printf("haha\n");
    int *b = ram_malloc(sizeof(int));
    assert(trk_trace(trace) == TRK_RESULT_OK);
    printf("haha\n");
    int *c = ram_malloc(sizeof(int));
    assert(trk_trace(trace) == TRK_RESULT_OK);
    printf("haha\n");
    ram_free(a);
    assert(trk_trace(trace) == TRK_RESULT_OK);
    printf("haha\n");
    ram_free(c);
    assert(trk_trace(trace) == TRK_RESULT_OK);
    printf("haha\n");
    ram_free(b);
    assert(trk_trace(trace) == TRK_RESULT_OK);
    printf("haha\n");*/

    struct List_int list_a = list_create(int, 10);
    struct List_int list_b = list_create(int, 10);
    struct List_List_int list_c = list_create(List_int, 10);

    assert(trk_trace(trace) == 0);
    printf("haha\n");

    int a = 1;
    list_int_append(&list_a, &a);
    list_a.data[0] = 2;
    printf("%lu %i\n", list_a.size, list_a.data[10]);
    list_List_int_append(&list_c, &list_a);
    printf("%lu %lu\n", list_c.size, list_c.data[10].size);

    list_List_int_destroy(&list_c);
    list_int_destroy(&list_b);
    list_int_destroy(&list_a);

    assert(trk_trace(trace) == 0);
    printf("haha\n");

    return 0;
}
