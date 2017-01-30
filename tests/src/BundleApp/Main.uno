using Uno;
using Uno.IO;

namespace BundleApp
{
    static class Program
    {
        static void Main(params string[] args)
        {
            debug_log "Arguments: " + args.Length;
            foreach (var a in args)
                debug_log "    " + a;

            BundleLib.Test.Assert();
            SubProject.Test.Assert();
            Assert("BundleFile.pdf");
            Assert("js/SubProjectBundleFile.js");
            Assert("js/BundleLibBundleFile.js");

            // This should not throw
            Bundle.Get();
        }

        static void Assert(string filepath)
        {
            foreach (var f in Bundle.AllFiles)
            {
                if (f.SourcePath == filepath)
                {
                    debug_log filepath + " Success!";
                    return;
                }
            }

            debug_log filepath + " Failed!";
        }
    }
}
