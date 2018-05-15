// @(MSG_ORIGIN)
// @(MSG_EDIT_WARNING)

#include <Uno/Support.h>
#include <mutex>
@{byte:IncludeDirective}
@{Uno.IntPtr:IncludeDirective}

#ifdef ANDROID
#include <android/log.h>
#elif defined(__APPLE__)
#include <TargetConditionals.h>
void uLogApple(const char* prefix, const char* format, va_list args);
#else
#include <cstdio>
#endif

#ifdef WIN32
#include <Uno/WinAPIHelper.h>
#else
#include <pthread.h>
#endif

static std::recursive_mutex _Critical;

// Synchronized logging
void uLogv(int level, const char* format, va_list args)
{
    U_ASSERT(uLogLevelDebug == 0 &&
             uLogLevelInformation == 1 &&
             uLogLevelWarning == 2 &&
             uLogLevelError == 3 &&
             uLogLevelFatal == 4);

    if (!format)
        format = "";

    if (level < 0)
        level = 0;
    else if (level > uLogLevelFatal)
        level = uLogLevelFatal;

#ifdef ANDROID
    int logs[] = {
        ANDROID_LOG_DEBUG,  // uLogLevelDebug
        ANDROID_LOG_INFO,   // uLogLevelInformation
        ANDROID_LOG_WARN,   // uLogLevelWarning
        ANDROID_LOG_ERROR,  // uLogLevelError
        ANDROID_LOG_FATAL   // uLogLevelFatal
    };
    __android_log_vprint(logs[level], "@(Project.Name)", format, args);
#else
    static const char* strings[] = {
        "",             // uLogLevelDebug
        "Info: ",       // uLogLevelInformation
        "Warning: ",    // uLogLevelWarning
        "Error: ",      // uLogLevelError
        "Fatal: "       // uLogLevelFatal
    };
#if TARGET_OS_IPHONE
    // Defined in ObjC file to call NSLog()
    uLogApple(strings[level], format, args);
#else
    FILE* fp = level >= uLogLevelWarning
            ? stderr
            : stdout;
    _Critical.lock();
    fputs(strings[level], fp);
    vfprintf(fp, format, args);
    fputc('\n', fp);
    fflush(fp);
    _Critical.unlock();
#endif
#endif
}

void uLog(int level, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    uLogv(level, format, args);
    va_end(args);
}

void uFatal(const char* src, const char* msg)
{
    uLog(uLogLevelFatal, "Runtime Error in %s: %s",
        src && strlen(src) ? src : "(unknown)",
        msg && strlen(msg) ? msg : "(no message)");
    abort();
}

uThreadLocal* uCreateThreadLocal(void (*destructor)(void*))
{
#ifdef WIN32
    // TODO: Handle destructor...
    return (uThreadLocal*)(intptr_t)::TlsAlloc();
#else
    pthread_key_t handle;
    if (pthread_key_create(&handle, destructor))
        U_THROW_IOE("pthread_key_create() failed");

    return (uThreadLocal*)(intptr_t)handle;
#endif
}

void uDeleteThreadLocal(uThreadLocal* handle)
{
#ifdef WIN32
    ::TlsFree((DWORD)(intptr_t)handle);
#else
    pthread_key_delete((pthread_key_t)(intptr_t)handle);
#endif
}

void uSetThreadLocal(uThreadLocal* handle, void* data)
{
#ifdef WIN32
    ::TlsSetValue((DWORD)(intptr_t)handle, data);
#else
    pthread_setspecific((pthread_key_t)(intptr_t)handle, data);
#endif
}

void* uGetThreadLocal(uThreadLocal* handle)
{
#ifdef WIN32
    return ::TlsGetValue((DWORD)(intptr_t)handle);
#else
    return pthread_getspecific((pthread_key_t)(intptr_t)handle);
#endif
}

bool uEnterCriticalIfNull(void* addr)
{
    if (*(void**)addr)
        return false;

    _Critical.lock();

    if (!*(void**)addr)
        return true;

    _Critical.unlock();
    return false;
}

void uEnterCritical()
{
    _Critical.lock();
}

void uExitCritical()
{
    _Critical.unlock();
}

#ifdef __APPLE__

#include <execinfo.h>
uArray* uGetNativeStackTrace(int skipFrames)
{
    void* callStack[64];
    int callStackDepth = backtrace(callStack, sizeof(callStack) / sizeof(callStack[0]));
    return uArray::New(@{Uno.IntPtr[]:TypeOf}, callStackDepth - skipFrames, callStack + skipFrames);
}

#elif defined(__GNUC__)

#include <unwind.h>

struct uUnwindState
{
    int _skipFrames;
    int _callStackDepth;
    void* _callStack[64];
};

static _Unwind_Reason_Code uUnwindCallback(struct _Unwind_Context* context, void *arg)
{
    uUnwindState* state = (uUnwindState*)arg;

    if (state->_skipFrames > 0)
    {
        state->_skipFrames--;
        return _URC_NO_REASON;
    }

    void* pc = (void*)_Unwind_GetIP(context);
    if (pc)
    {
        if (state->_callStackDepth == sizeof(state->_callStack) / sizeof(state->_callStack[0]))
            return _URC_END_OF_STACK;
        else
            state->_callStack[state->_callStackDepth++] = pc;
    }
    return _URC_NO_REASON;
}

// Use GCC's libunwind

uArray* uGetNativeStackTrace(int skipFrames)
{
    uUnwindState state = {
        skipFrames,
        0,
        { 0 }
    };

    _Unwind_Backtrace(uUnwindCallback, &state);
    return uArray::New(@{Uno.IntPtr[]:TypeOf}, state._callStackDepth, state._callStack);
}

#elif defined(WIN32)

// Windows provides a simple API for this
uArray* uGetNativeStackTrace(int skipFrames)
{
    void* callStack[64];
    int callStackDepth = CaptureStackBackTrace(skipFrames, sizeof(callStack) / sizeof(callStack[0]), callStack, NULL);
    return uArray::New(@{Uno.IntPtr[]:TypeOf}, callStackDepth, callStack);
}

#else

// last resort, we don't have any way of getting a native stack-trace, return NULL :(
uArray* uGetNativeStackTrace(int skipFrames)
{
    return NULL;
}

#endif

// Generated by _invoke.py
#define CALL ((void(*)
#define ME )func)
#define T void*
#define A args

void uInvoke(const void* func, void** args, size_t count)
{
    switch (count)
    {
    default: U_THROW_IOORE();
    case  0: CALL() ME(); break;
    case  1: CALL(T) ME(A[0]); break;
    case  2: CALL(T,T) ME(A[0],A[1]); break;
    case  3: CALL(T,T,T) ME(A[0],A[1],A[2]); break;
    case  4: CALL(T,T,T,T) ME(A[0],A[1],A[2],A[3]); break;
    case  5: CALL(T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4]); break;
    case  6: CALL(T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5]); break;
    case  7: CALL(T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6]); break;
    case  8: CALL(T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7]); break;
    case  9: CALL(T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8]); break;
    case 10: CALL(T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[9]); break;
    case 11: CALL(T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[10])
             ; break;
    case 12: CALL(T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[
             10],A[11]); break;
    case 13: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[
             10],A[11],A[12]); break;
    case 14: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[9],
             A[10],A[11],A[12],A[13]); break;
    case 15: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[
             9],A[10],A[11],A[12],A[13],A[14]); break;
    case 16: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],
             A[9],A[10],A[11],A[12],A[13],A[14],A[15]); break;
    case 17: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8]
             ,A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[16]); break;
    case 18: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[
             8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[16],A[17]); break;
    case 19: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],
             A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[16],A[17],A[18]); break;
    case 20: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[
             7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[16],A[17],A[18],A[19]); break;
    case 21: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],
             A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[16],A[17],A[18],A[19],A[20]);
             break;
    case 22: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6]
             ,A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[16],A[17],A[18],A[19],A[20],A[21]
             ); break;
    case 23: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[
             6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[16],A[17],A[18],A[19],A[20],A[
             21],A[22]); break;
    case 24: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],
             A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[16],A[17],A[18],A[19],A[20],
             A[21],A[22],A[23]); break;
    case 25: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[
             5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[16],A[17],A[18],A[19],A[
             20],A[21],A[22],A[23],A[24]); break;
    case 26: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],
             A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[16],A[17],A[18],A[19],A[
             20],A[21],A[22],A[23],A[24],A[25]); break;
    case 27: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4]
             ,A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[16],A[17],A[18],A[19],
             A[20],A[21],A[22],A[23],A[24],A[25],A[26]); break;
    case 28: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[
             4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[16],A[17],A[18],A[19]
             ,A[20],A[21],A[22],A[23],A[24],A[25],A[26],A[27]); break;
    case 29: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],
             A[4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[16],A[17],A[18],A[
             19],A[20],A[21],A[22],A[23],A[24],A[25],A[26],A[27],A[28]); break;
    case 30: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[
             3],A[4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[16],A[17],A[18],
             A[19],A[20],A[21],A[22],A[23],A[24],A[25],A[26],A[27],A[28],A[29]); break;
    case 31: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2],
             A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[16],A[17],A[
             18],A[19],A[20],A[21],A[22],A[23],A[24],A[25],A[26],A[27],A[28],A[29],A[30]); break;
    case 32: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[2]
             ,A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[16],A[17],A[
             18],A[19],A[20],A[21],A[22],A[23],A[24],A[25],A[26],A[27],A[28],A[29],A[30],A[31]);
             break;
    case 33: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],A[
             2],A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[16],A[17],
             A[18],A[19],A[20],A[21],A[22],A[23],A[24],A[25],A[26],A[27],A[28],A[29],A[30],A[31],A[
             32]); break;
    case 34: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[1],
             A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[16],A[17]
             ,A[18],A[19],A[20],A[21],A[22],A[23],A[24],A[25],A[26],A[27],A[28],A[29],A[30],A[31],A[
             32],A[33]); break;
    case 35: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],A[
             1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[16],A[
             17],A[18],A[19],A[20],A[21],A[22],A[23],A[24],A[25],A[26],A[27],A[28],A[29],A[30],A[31]
             ,A[32],A[33],A[34]); break;
    case 36: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0],
             A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[16],
             A[17],A[18],A[19],A[20],A[21],A[22],A[23],A[24],A[25],A[26],A[27],A[28],A[29],A[30],A[
             31],A[32],A[33],A[34],A[35]); break;
    case 37: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[0]
             ,A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[16]
             ,A[17],A[18],A[19],A[20],A[21],A[22],A[23],A[24],A[25],A[26],A[27],A[28],A[29],A[30],A[
             31],A[32],A[33],A[34],A[35],A[36]); break;
    case 38: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(A[
             0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],A[
             16],A[17],A[18],A[19],A[20],A[21],A[22],A[23],A[24],A[25],A[26],A[27],A[28],A[29],A[30]
             ,A[31],A[32],A[33],A[34],A[35],A[36],A[37]); break;
    case 39: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T) ME(
             A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[15],
             A[16],A[17],A[18],A[19],A[20],A[21],A[22],A[23],A[24],A[25],A[26],A[27],A[28],A[29],A[
             30],A[31],A[32],A[33],A[34],A[35],A[36],A[37],A[38]); break;
    case 40: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T)
             ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[
             15],A[16],A[17],A[18],A[19],A[20],A[21],A[22],A[23],A[24],A[25],A[26],A[27],A[28],A[29]
             ,A[30],A[31],A[32],A[33],A[34],A[35],A[36],A[37],A[38],A[39]); break;
    case 41: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T)
             ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],A[
             15],A[16],A[17],A[18],A[19],A[20],A[21],A[22],A[23],A[24],A[25],A[26],A[27],A[28],A[29]
             ,A[30],A[31],A[32],A[33],A[34],A[35],A[36],A[37],A[38],A[39],A[40]); break;
    case 42: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,
             T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14],
             A[15],A[16],A[17],A[18],A[19],A[20],A[21],A[22],A[23],A[24],A[25],A[26],A[27],A[28],A[
             29],A[30],A[31],A[32],A[33],A[34],A[35],A[36],A[37],A[38],A[39],A[40],A[41]); break;
    case 43: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,
             T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[14]
             ,A[15],A[16],A[17],A[18],A[19],A[20],A[21],A[22],A[23],A[24],A[25],A[26],A[27],A[28],A[
             29],A[30],A[31],A[32],A[33],A[34],A[35],A[36],A[37],A[38],A[39],A[40],A[41],A[42]);
             break;
    case 44: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,
             T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],A[
             14],A[15],A[16],A[17],A[18],A[19],A[20],A[21],A[22],A[23],A[24],A[25],A[26],A[27],A[28]
             ,A[29],A[30],A[31],A[32],A[33],A[34],A[35],A[36],A[37],A[38],A[39],A[40],A[41],A[42],A[
             43]); break;
    case 45: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,
             T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13],
             A[14],A[15],A[16],A[17],A[18],A[19],A[20],A[21],A[22],A[23],A[24],A[25],A[26],A[27],A[
             28],A[29],A[30],A[31],A[32],A[33],A[34],A[35],A[36],A[37],A[38],A[39],A[40],A[41],A[42]
             ,A[43],A[44]); break;
    case 46: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,
             T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[13]
             ,A[14],A[15],A[16],A[17],A[18],A[19],A[20],A[21],A[22],A[23],A[24],A[25],A[26],A[27],A[
             28],A[29],A[30],A[31],A[32],A[33],A[34],A[35],A[36],A[37],A[38],A[39],A[40],A[41],A[42]
             ,A[43],A[44],A[45]); break;
    case 47: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,
             T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],A[
             13],A[14],A[15],A[16],A[17],A[18],A[19],A[20],A[21],A[22],A[23],A[24],A[25],A[26],A[27]
             ,A[28],A[29],A[30],A[31],A[32],A[33],A[34],A[35],A[36],A[37],A[38],A[39],A[40],A[41],A[
             42],A[43],A[44],A[45],A[46]); break;
    case 48: CALL(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,
             T,T,T,T,T,T,T) ME(A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7],A[8],A[9],A[10],A[11],A[12],
             A[13],A[14],A[15],A[16],A[17],A[18],A[19],A[20],A[21],A[22],A[23],A[24],A[25],A[26],A[
             27],A[28],A[29],A[30],A[31],A[32],A[33],A[34],A[35],A[36],A[37],A[38],A[39],A[40],A[41]
             ,A[42],A[43],A[44],A[45],A[46],A[47]); break;
    }
}
