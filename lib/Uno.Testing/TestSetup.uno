using Uno;
using Uno.Diagnostics;
using Uno.Platform;
using Uno.Testing;
using Uno.Threading;
using System.Globalization;

namespace Uno.Testing
{
    public sealed class TestSetup
    {
        readonly AbstractRunner _runner;

        public TestSetup(Registry registry)
        {
            if defined(ANDROID)
            {
                // Increase the chance for uno to connect before we're done
                // This is required because `uno build --run` on Android have
                // have problems outputting log of a short-running process.
                //
                // The 2500ms duration was chosen as the problem occured once
                // during testing at 1500ms, but never after increasing to this value.
                Thread.Sleep(2500);
            }
            else if defined(DOTNET)
            {
                // Use invariant culture when running tests on .NET
                Thread.CurrentThread.CurrentCulture = CultureInfo.InvariantCulture;
            }

            _runner = new RemoteRunner(registry);
        }

        public void RunAll()
        {
            _runner.EnsureStarted();

            while (_runner.NextTest != null)
            {
                _runner.Update();
                Uno.Platform.Displays.MainDisplay.OnTick(new TimerEventArgs(0, 0, 0));
            }
        }
    }
}
