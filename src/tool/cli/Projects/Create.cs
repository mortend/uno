using System;
using System.Collections.Generic;
using System.IO;
using Mono.Options;
using Uno.IO;
using Uno.ProjectFormat;

namespace Uno.CLI.Projects
{
    class Create : Command
    {
        public override string Name => "create";
        public override string Description => "Create a new project.";

        public override void Help()
        {
            WriteUsage("[options] [file|directory]");

            WriteHead("Available options");
            WriteRow("-c, --class=NAME",    "Specify program class name", true);
            WriteRow("-n, --name=NAME",     "Specify project file name", true);
            WriteRow("-d, --defaults",      "Add default settings");
            WriteRow("-e, --empty",         "Create empty project without packages or items");
            WriteRow("-f, --force",         "Overwrite any existing project without warning");
            WriteRow("    --flatten",       "Flatten items to explicit list of files");
        }

        public override void Execute(IEnumerable<string> args)
        {
            var force = false;
            string projectName = null;
            var className = "Program";
            var empty = false;
            var flatten = false;
            var defaults = false;

            var input = new OptionSet {
                    { "c|class=",   value => className = value },
                    { "n|name=",    value => projectName = value },
                    { "d|defaults", value => defaults = true },
                    { "e|empty",    value => empty = true },
                    { "f|force",    value => force = true },
                    { "flatten",    value => flatten = true }
                }.Parse(args);

            if (input.Count == 0)
                input.Add(".");
            else
                input.CheckArguments();

            if (input.Count != 1 || string.IsNullOrWhiteSpace(input[0]))
                throw new ArgumentException("Expected one project [file|directory]");

            var fullName = input[0].ToFullPath();

            if (projectName == null)
                projectName = Path.GetFileName(fullName);

            if (!fullName.ToUpperInvariant().EndsWith(".UNOPROJ"))
                fullName = Path.Combine(fullName, projectName + ".unoproj");

            if (Directory.Exists(fullName))
                throw new Exception(fullName.Quote() + " is a directory");

            if (!force && File.Exists(fullName))
                throw new Exception(fullName.Quote() + " already exists");

            var dirName = Path.GetDirectoryName(fullName);
            Disk.CreateDirectory(dirName);

            WriteLine("Creating project " + fullName.Quote());

            var project = new Project(fullName);

            if (defaults)
                project.AddDefaults();

            if (!empty)
            {
                project.MutableIncludeItems.Add("*");

                foreach (var e in project.Config.GetStringArray("Packages.Default") ?? new string[0])
                    project.MutablePackageReferences.Add(e);

                var fileName = Path.Combine(project.RootDirectory, className + ".uno");
                File.WriteAllText(fileName, string.Format(ProgramTemplate, project.Name.ToIdentifier(true), className.ToIdentifier()));
            }

            if (flatten)
                project.FlattenItems(Log);

            project.Save();
        }

        const string ProgramTemplate =
@"using Uno;

namespace {0}
{{
    class {1}
    {{
        static int Main(string[] args)
        {{
            debug_log ""Hello, World!"";
            return 0;
        }}
    }}
}}
";
    }
}
