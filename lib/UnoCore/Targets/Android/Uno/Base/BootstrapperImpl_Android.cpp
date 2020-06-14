#include <BootstrapperImpl_Android.h>
#include <@{Android.Base.JNI:include}>
#include <@{Android.Base.Types.Bridge:include}>
@(Android.Bindings.Bootstrapper.Include:join())
void BootstrapperImpl() {
    @(Android.Bindings.Bootstrapper.Statement:join())
}
