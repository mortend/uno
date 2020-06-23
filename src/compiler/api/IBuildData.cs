﻿using Uno.Compiler.API.Domain.Extensions;
using Uno.Compiler.API.Domain.IL;
using Uno.Compiler.API.Domain.IL.Members;
using Uno.Compiler.API.Domain.IL.Statements;

namespace Uno.Compiler.API
{
    public interface IBuildData
    {
        Namespace IL { get; }
        Method Entrypoint { get; }
        ExtensionRoot Extensions { get; }
    }
}
