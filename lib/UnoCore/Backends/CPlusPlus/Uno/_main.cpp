// @(MSG_ORIGIN)
// @(MSG_EDIT_WARNING)

#include <Uno/ObjectModel.h>
@(Main.IncludeDirective)
@(TypeObjects.Declaration:JoinSorted())
void uInitRtti(uType*(*factories[])());

// See @{Uno.Environment.GetCommandLineArgs():Call()}
int uArgc = 0;
char** uArgv = NULL;

void uInit(int argc, char* argv[])
{
    uArgc = argc;
    uArgv = argv;
    uType* (*factories[])() = {
        @(TypeObjects.FunctionPointer:JoinSorted('\n        ', '', ','))
        nullptr
    };

    uInitRtti(factories);
}

#if @(ANDROID:Defined)
#include <jni.h>
#include <Uno/JNIHelper.h>

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
    {
        uLog(uLogLevelFatal, "JavaVM->GetEnv() failed");
        return -1;
    }

    jclass activityClass = env->FindClass("@(Activity.Package:Replace('.', '/'))/@(Activity.Name)");
    jclass nativeExternClass = env->FindClass("com/Bindings/ExternedBlockHost");

    if (!activityClass)
        U_FATAL("COULD NOT FIND ACTIVITY CLASS");

    if (!nativeExternClass)
        U_FATAL("COULD NOT FIND NATIVEEXTERNCLASS CLASS");

    JniHelper::Init(vm, env, activityClass, nativeExternClass);

    static uRuntime uno;
    uAutoReleasePool pool;
    uInit(0, NULL);

    int retval = @(Main.Entrypoint);
    if (retval != 0)
    {
        uLog(uLogLevelError, "Exited with code: %d", retval);
        return -1;
    }

    return JNI_VERSION_1_6;
}

#else

int main(int argc, char* argv[])
{
    uRuntime uno;
    uAutoReleasePool pool;
    uInit(argc, argv);
    return @(Main.Entrypoint);
}

#endif
