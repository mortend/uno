using Uno.Compiler.ExportTargetInterop;

namespace System
{
    [DotNetType]
    public extern(DOTNET) class String
    {
        public static extern string Format(string format, object arg0);
    }
}
