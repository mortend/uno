using System;
using System.Globalization;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows.Forms;

namespace Uno.AppLoader
{
    public static class Program
    {
        [STAThread]
        static void Main()
        {
            System.Windows.Forms.Application.EnableVisualStyles();
            System.Windows.Forms.Application.SetCompatibleTextRenderingDefault(false);
            DpiAwareness.SetDpiAware(DpiAwareness.ProcessDpiAwareness.SystemAware);
            new MainForm(UnoGenerated).MainLoop();
        }

        static void UnhandledExceptionTrapper(object sender, UnhandledExceptionEventArgs e)
        {
            MessageBox.Show("The application has crashed because of an unhandled exception:\n\n" + e.ExceptionObject, "Fatal Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
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
                    // Set DllImportResolver for loading native libraries used in generated assemblies
                    NativeLibrary.SetDllImportResolver(ev.LoadedAssembly, (libraryName, assembly, searchPath) => {
                        if (!libraryName.Contains(".dll") || libraryName.StartsWith("/"))
                            return IntPtr.Zero;

                        var dir = Path.GetDirectoryName(assembly.Location);
                        var lib = Path.Combine(dir, libraryName);

                        if (File.Exists(lib))
                            return LoadLibrary(lib);

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

        static void UnoGenerated()
        {
            // Uno compiler will replace this.
            new DummyApp();
        }

        [DllImport("kernel32.dll")]
        static extern IntPtr LoadLibrary(string filename);
    }
}
