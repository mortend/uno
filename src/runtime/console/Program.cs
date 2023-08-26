using OpenGL;
using System;
using System.Globalization;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Versioning;
using System.Threading;
using Uno.AppLoader.Dummy;
using Uno.Diagnostics;
using Uno.Platform;
using Uno.Platform.Internal;

namespace Uno.AppLoader
{
    class Program
    {
        [STAThread]
        static void Main()
        {
            GL.Initialize(new DummyGL());
            GraphicsContextBackend.SetInstance(new DummyGC());
            WindowBackend.SetInstance(new DummyWindow());

            UnoGenerated();
            DotNetApplication.Start();

            const double targetTime = 1.0 / 60;

            while (Application.Current != null)
            {
                var startTime = Clock.GetSeconds();

                Bootstrapper.OnUpdate();
                Bootstrapper.OnDraw();

                var renderTime = Clock.GetSeconds() - startTime;
                var msTimeout = (int)((targetTime - renderTime) * 1000.0 + 0.5);

                if (msTimeout > 0)
                    Thread.Sleep(msTimeout);
            }
        }

        static void UnoGenerated()
        {
            // The Uno compiler will replace this.
        }

        static void UnhandledExceptionTrapper(object sender, UnhandledExceptionEventArgs e)
        {
            Console.Error.WriteLine("The application has crashed because of an unhandled exception:\n\n" + e.ExceptionObject);
            Environment.Exit(1);
        }

        static Program()
        {
            CultureInfo.DefaultThreadCurrentCulture = CultureInfo.InvariantCulture;
            CultureInfo.DefaultThreadCurrentUICulture = CultureInfo.InvariantCulture;
            Thread.CurrentThread.CurrentCulture = CultureInfo.InvariantCulture;
            Thread.CurrentThread.CurrentUICulture = CultureInfo.InvariantCulture;
            AppDomain.CurrentDomain.UnhandledException += UnhandledExceptionTrapper;

            AppDomain.CurrentDomain.AssemblyLoad += (sender, ev) => {
                try
                {
                    var asm = ev.LoadedAssembly;

                    if (!GetDllImportResolverEnabled(asm))
                        return;

                    // Set DllImportResolver for loading native libraries used in generated assemblies
                    NativeLibrary.SetDllImportResolver(asm, (libraryName, assembly, searchPath) => {
                        if (!libraryName.Contains(".dll") || libraryName.StartsWith("/"))
                            return IntPtr.Zero;

                        var dir = Path.GetDirectoryName(assembly.Location);
                        var lib = Path.Combine(dir,
                            OperatingSystem.IsWindows()
                                ? libraryName
                                : "lib" + libraryName.Replace(".dll",
                                    OperatingSystem.IsMacOS()
                                        ? ".dylib"
                                        : ".so"
                            ));

                        if (File.Exists(lib))
                        {
                            if (OperatingSystem.IsWindows())
                                return LoadLibrary(lib);
                            else if (OperatingSystem.IsMacOS())
                                return Dlfcn.dlopen(lib, 0);
                            else
                                return dlopen(lib, 0);
                        }

                        Console.Error.WriteLine("Native library not found: " + lib);
                        return IntPtr.Zero;
                    });
                }
                catch (Exception e)
                {
                    Console.Error.WriteLine("NativeLibrary.SetDllImportResolver: " + e);
                }
            };
        }

        static bool GetDllImportResolverEnabled(Assembly asm)
        {
            foreach (var attr in asm.GetCustomAttributes())
            {
                var metadata = attr as AssemblyMetadataAttribute;

                if (metadata?.Key == "DllImportResolver")
                    return bool.Parse(metadata.Value);
            }

            return false;
        }

        // Source: https://github.com/xamarin/xamarin-macios/blob/ffdbbbb698ae33981d215bb7b92d7d658b3c1501/src/ObjCRuntime/Dlfcn.cs
        [SupportedOSPlatform("macos")]
        static class Dlfcn
        {
            [Flags]
            public enum Mode : int
            {
                None = 0x0,
                Lazy = 0x1,
                Now = 0x2,
                Local = 0x4,
                Global = 0x8,
                NoLoad = 0x10,
                NoDelete = 0x80,
                First = 0x100,
            }

            [DllImport("/usr/lib/libSystem.dylib", EntryPoint = "dlopen")]
            static extern IntPtr _dlopen(IntPtr path, Mode mode /* this is int32, not nint */);

            internal static IntPtr _dlopen(string path, Mode mode /* this is int32, not nint */)
            {
                using var pathPtr = new TransientString(path);
                return _dlopen(pathPtr, mode);
            }

            public static IntPtr dlopen(string path, int mode)
            {
                return dlopen(path, mode, showWarning: true);
            }

            public static IntPtr dlopen(string path, Mode mode)
            {
                return _dlopen(path, mode);
            }

            static bool warningShown;
            // the linker can eliminate the body of this method (and the above static variable) on release builds
            static void WarnOnce()
            {
                if (!warningShown)
                    Console.Error.WriteLine("You are using dlopen without a full path, retrying by prepending /usr/lib");
                warningShown = true;
            }

            internal static IntPtr dlopen(string path, int mode, bool showWarning)
            {
                var x = _dlopen(path, (Mode)mode);
                if (x != IntPtr.Zero)
                    return x;

                // In iOS < 9, you could dlopen ("libc") and that would work.
                // In iOS >= 9, this fails with:
                // "no cache image with name (<top>)"
                if (path?.IndexOf('/') == -1)
                {
                    if (showWarning)
                        WarnOnce();
                    return dlopen("/usr/lib/" + path, mode, false);
                }
                return IntPtr.Zero;
            }

            internal struct TransientString : IDisposable
            {
                IntPtr ptr;
                public enum Encoding
                {
                    Auto = 0,
                    BStr,
                    Ansi, // aka LPStr
                    Unicode,
                };

                public TransientString(nint size)
                {
                    ptr = Marshal.AllocHGlobal((IntPtr)size);
                }

                public TransientString(string str, Encoding encoding = Encoding.Auto)
                {
                    switch (encoding)
                    {
                        case Encoding.Auto:
                            ptr = Marshal.StringToHGlobalAuto(str);
                            break;
                        case Encoding.BStr:
                            ptr = Marshal.StringToBSTR(str);
                            break;
                        case Encoding.Ansi:
                            ptr = Marshal.StringToHGlobalAnsi(str);
                            break;
                        case Encoding.Unicode:
                            ptr = Marshal.StringToHGlobalUni(str);
                            break;
                        default:
                            throw new ArgumentOutOfRangeException(nameof(encoding));
                    }
                }

                public void Dispose()
                {
                    if (ptr != IntPtr.Zero)
                    {
                        Marshal.FreeHGlobal(ptr);
                        ptr = IntPtr.Zero;
                    }
                }

                public static implicit operator IntPtr(TransientString str) => str.ptr;
                public static explicit operator string(TransientString str) => Marshal.PtrToStringAuto(str.ptr);

                public static string ToStringAndFree(IntPtr ptr, Encoding encoding = Encoding.Auto)
                {
                    string result = null;
                    switch (encoding)
                    {
                        case Encoding.Auto:
                            result = Marshal.PtrToStringAuto(ptr);
                            break;
                        case Encoding.BStr:
                            result = Marshal.PtrToStringBSTR(ptr);
                            break;
                        case Encoding.Ansi:
                            result = Marshal.PtrToStringAnsi(ptr);
                            break;
                        case Encoding.Unicode:
                            result = Marshal.PtrToStringUni(ptr);
                            break;
                        default:
                            throw new ArgumentOutOfRangeException(nameof(encoding));
                    }
                    Marshal.FreeHGlobal(ptr);
                    return result;
                }

                public static IntPtr AllocStringArray(string[] arr, Encoding encoding = Encoding.Auto)
                {
                    if (arr is null)
                        return IntPtr.Zero;
                    var ptrArr = Marshal.AllocHGlobal(arr.Length * IntPtr.Size);
                    var offset = 0;
                    var step = IntPtr.Size;
                    for (int i = 0; i < arr.Length; i++, offset += step)
                    {
                        var str = arr[i] is null ? IntPtr.Zero : new TransientString(arr[i], encoding);
                        Marshal.WriteIntPtr(ptrArr + offset, str);
                    }
                    return ptrArr;
                }

                public static void FreeStringArray(IntPtr ptrArr, int count)
                {
                    if (ptrArr == IntPtr.Zero)
                        return;
                    var offset = 0;
                    var step = IntPtr.Size;
                    for (int i = 0; i < count; i++, offset += step)
                    {
                        var str = Marshal.ReadIntPtr(ptrArr + offset);
                        if (str != IntPtr.Zero)
                            Marshal.FreeHGlobal(Marshal.ReadIntPtr(ptrArr + offset));
                    }
                    Marshal.FreeHGlobal(ptrArr);
                }
            }
        }

        [DllImport("libdl.so")]
        static extern IntPtr dlopen(string filename, int flags);

        [DllImport("kernel32.dll")]
        [SupportedOSPlatform("windows")]
        static extern IntPtr LoadLibrary(string filename);
    }
}
