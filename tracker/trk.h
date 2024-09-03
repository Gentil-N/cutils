#ifndef __TRK_H__
#define __TRK_H__

#ifdef __cplusplus
extern "C"
{
#endif

enum TrkResult
{
    TRK_RESULT_OK = 0,
    TRK_RESULT_ERROR_ADDRESS_NOT_FOUND = 1,
    TRK_RESULT_ERROR_OUT_OF_MEMORY = 2,
    TRK_RESULT_ERROR_INTERNAL_PTHREAD = 3,
};

typedef void (*trk_trace_fn)(void *address, const char *file, int line);

enum TrkResult _trk_register(void *address, const char *file, int line);
enum TrkResult _trk_unregister(void *address);
enum TrkResult _trk_change_register(void *old_address, void *new_address);
enum TrkResult _trk_trace(trk_trace_fn fn);

#ifndef NDEBUG
#define trk_register(address, file, line) _trk_register(address, file, line)
#define trk_unregister(address) _trk_unregister(address)
#define trk_change_register(old_address, new_address) _trk_change_register(old_address, new_address)
#define trk_trace(fn) _trk_trace(fn)
#else
#define trk_register(address, file, line)
#define trk_unregister(address)
#define trk_change_register(old_address, new_address)
#define trk_trace(fn)
#endif // NDEBUG

#ifdef __cplusplus
}
#endif

#endif // __TRK_H__
