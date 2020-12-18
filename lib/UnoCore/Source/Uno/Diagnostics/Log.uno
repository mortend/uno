using Uno.Compiler.ExportTargetInterop;
using System;

namespace Uno.Diagnostics
{
    public enum LogLevel
    {
        Debug,
        Information,
        Warning,
        Error,
        Fatal,
    }

    public static class Log
    {
        extern(PREVIEW)
        static Action<string, LogLevel> _handler;

        extern(PREVIEW)
        public static void SetHandler(Action<string, LogLevel> handler)
        {
            _handler = handler;
        }

        public static void Debug(string message)
        {
            WriteLine(message, LogLevel.Debug);
        }

        public static void Debug(string format, params object[] args)
        {
            WriteLine(string.Format(format, args), LogLevel.Debug);
        }

        public static void Information(string message)
        {
            WriteLine(message, LogLevel.Information);
        }

        public static void Information(string format, params object[] args)
        {
            WriteLine(string.Format(format, args), LogLevel.Information);
        }

        public static void Warning(string message)
        {
            WriteLine(message, LogLevel.Warning);
        }

        public static void Warning(string format, params object[] args)
        {
            WriteLine(string.Format(format, args), LogLevel.Warning);
        }

        public static void Error(string message)
        {
            WriteLine(message, LogLevel.Error);
        }

        public static void Error(string format, params object[] args)
        {
            WriteLine(string.Format(format, args), LogLevel.Error);
        }

        public static void Fatal(string message)
        {
            WriteLine(message, LogLevel.Fatal);
        }

        public static void Fatal(string format, params object[] args)
        {
            WriteLine(string.Format(format, args), LogLevel.Fatal);
        }

        public static void WriteLine(string message, LogLevel level)
        {
            if defined(PREVIEW)
                if (_logHandler != null)
                    _logHandler(message, level);

            if defined(CPLUSPLUS)
            @{
                uCString cstr($0);
                uLog($1, "%s", cstr.Ptr);
            @}
            else if defined(DOTNET)
            {
                if (level == 0)
                    Console.WriteLine(message);
                else if ((int) level < LogLevel.Warning)
                    Console.Out.WriteLine(level + ": " + message);
                else
                    Console.Error.WriteLine(level + ": " + message);
            }
            else
                build_error;
        }
    }
}
