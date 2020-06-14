:: @(MSG_ORIGIN)
:: @(MSG_EDIT_WARNING)
@echo off
pushd "%~dp0"
setlocal

#if !@(SDK.Directory:isset) || !@(NDK.Directory:isset)
echo ERROR: Could not locate the Android SDK or NDK. >&2
echo. >&2
echo These dependencies can be acquired by installing 'android-build-tools': >&2
echo. >&2
echo     npm install android-build-tools -g >&2
echo. >&2
echo After installing, pass --force to make sure the new configuration is picked up. >&2
echo. >&2
echo     uno build android --force >&2
echo. >&2
goto ERROR
#endif

#if @(JDK.Directory:isset)
set JAVA_HOME=@(JDK.Directory:nativepath)
#endif

:: Make sure ninja.exe and cmake.exe are in PATH.
for /D %%D in ("@(SDK.Directory:nativepath)\cmake\*") do (
    if exist "%%D\bin" (
        echo Using %%D
        set PATH="%%D\bin;%PATH%"
        goto BUILD
    )
)

:BUILD
call gradlew @(Gradle.AssembleTask) %* || goto ERROR

#if @(LIBRARY:defined)
copy /Y @(Outputs.AAR:quoteSpace:replace('/', '\\')) @(Product:quoteSpace) || goto ERROR
#else
copy /Y @(Outputs.APK:quoteSpace:replace('/', '\\')) @(Product:quoteSpace) || goto ERROR
# if !@(DEBUG:defined)
call gradlew @(Gradle.BundleTask) %* || goto ERROR
copy /Y @(Outputs.Bundle:quoteSpace:replace('/', '\\')) @(Bundle:quoteSpace) || goto ERROR
# endif
#endif

popd && exit /b 0

:ERROR
popd && exit /b 1
