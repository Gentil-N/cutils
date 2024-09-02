#ifndef __ERR_H__
#define __ERR_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define ERR_DEFINE(suffix)                                                              \
typedef void (*suffix##_error_callback_fn)(int code, const char *file, int line);       \
int suffix##_set_error_callback(suffix##_error_callback_fn fn);                         \
int _##suffix##_throw_error_fl(int code, const char *file, int line);                   \

#define ERR_DECLARE(suffix)                                                     \
static int suffix##_ERR_INIT = 0;                                               \
static pthread_mutex_t suffix##_ERR_MUTEX = {0};                                \
static suffix##_error_callback_fn suffix##_ERR_CALLBACK = NULL;                 \
int suffix##_set_error_callback(suffix##_error_callback_fn fn)                  \
{                                                                               \
    if (fn == NULL && suffix##_ERR_INIT == 1)                                   \
    {                                                                           \
        suffix##_ERR_INIT = 0;                                                  \
        suffix##_ERR_CALLBACK = NULL;                                           \
        if (pthread_mutex_destroy(&(suffix##_ERR_MUTEX)) != 0) return 1;        \
    }                                                                           \
    else if (suffix##_ERR_INIT == 0)                                            \
    {                                                                           \
        suffix##_ERR_INIT = 1;                                                  \
        suffix##_ERR_CALLBACK = fn;                                             \
        if (pthread_mutex_init(&(suffix##_ERR_MUTEX), NULL) != 0) return 1;     \
    }                                                                           \
    return 0;                                                                   \
}                                                                               \
int _##suffix##_throw_error_fl(int code, const char *file, int line)            \
{                                                                               \
    if(suffix##_ERR_INIT == 1 && suffix##_ERR_CALLBACK != NULL)                 \
    {                                                                           \
        if (pthread_mutex_lock(&(suffix##_ERR_MUTEX)) != 0) return 1;           \
        (suffix##_ERR_CALLBACK)(code, file, line);                              \
        if (pthread_mutex_unlock(&(suffix##_ERR_MUTEX)) != 0) return 1;         \
    }                                                                           \
    return 0;                                                                   \
}                                                                               \


#ifdef __cplusplus
}
#endif

#endif //__ERR_H__
