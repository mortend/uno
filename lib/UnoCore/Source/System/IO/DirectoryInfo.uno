using Uno.Compiler.ExportTargetInterop;

namespace System.IO
{
    [DotNetType]
    extern(DOTNET) public sealed class DirectoryInfo : FileSystemInfo
    {
        public extern DirectoryInfo(string path);
        private extern DirectoryInfo();
    }
}
