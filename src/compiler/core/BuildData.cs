using Uno.Compiler.API;
using Uno.Compiler.API.Domain;
using Uno.Compiler.API.Domain.Extensions;
using Uno.Compiler.API.Domain.IL;
using Uno.Compiler.API.Domain.IL.Expressions;
using Uno.Compiler.API.Domain.IL.Members;
using Uno.Compiler.API.Domain.IL.Statements;
using Uno.Compiler.API.Domain.IL.Types;
using Uno.Compiler.Core.IL;
using Uno.Compiler.Core.IL.Utilities;
using Uno.Logging;

namespace Uno.Compiler.Core
{
    public class BuildData : LogObject, IBuildData
    {
        public ExtensionRoot Extensions { get; }
        public Namespace IL { get; }
        public Method Entrypoint { get; private set; }

        internal BuildData(
            Log log,
            Namespace il,
            ExtensionRoot extensions)
            : base(log)
        {
            IL = il;
            Extensions = extensions;
        }

        public void SetEntrypoint(Method entrypoint)
        {
            Entrypoint = entrypoint;
        }
    }
}
