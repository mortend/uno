using System.Collections.Generic;
using System.Linq;
using IKVM.Reflection;
using Uno.Compiler.API.Backends;

namespace Uno.Compiler.Backends.CIL
{
    public class CilResult : BackendResult
    {
        readonly SortedSet<Location> _locations;

        public string AssemblyName { get; }

        internal CilResult(Assembly asm, SortedSet<Location> loc)
        {
            AssemblyName = asm.GetName().Name;
            _locations = loc;
        }

        public Location GetLocation(int ilOffset)
        {
            return _locations.TakeWhile(l => l.ILOffset < ilOffset).FirstOrDefault();
        }
    }
}
