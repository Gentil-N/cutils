#ifndef __LOG_H__
#define __LOG_H__

#ifdef __cplusplus
extern "C"
{
#endif

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

#ifdef __cplusplus
}
#endif

#endif // __LOG_H__
