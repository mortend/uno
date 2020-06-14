#include <BootstrapperImpl_Android.h>
#include <@{Android.Base.JNI:Include}>
#include <@{Android.Base.Types.Bridge:Include}>
@(Android.Bindings.Bootstrapper.Include:join())
void BootstrapperImpl() {
    @(Android.Bindings.Bootstrapper.Statement:join())
}
