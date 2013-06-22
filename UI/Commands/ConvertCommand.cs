using System.IO;
using Converter.UI.Settings;
using System.Diagnostics;
using System;

namespace Converter.UI.Commands
{
    public class ConvertCommand : CommandBase
    {
        public ConvertCommand(ConverterOptions options)
            : base(options)
        {
        }

        protected override bool OnCanExecute(object parameter)
        {
            return true;
        }

        protected override void OnExecute(object parameter)
        {
            var converterProcess = new ProcessStartInfo
            {
                FileName = this.Options.Converter,
                UseShellExecute = false, // According to http://msdn.microsoft.com/en-us/library/system.diagnostics.process.standardoutput.aspx
                RedirectStandardOutput = true // these two properties must be set to true to capture any standard output from the converter.
            };

            var process = Process.Start(converterProcess);

            process.WaitForExit();
            var result = process.StandardOutput.ReadToEnd();
            //TODO: Log result
        }
    }
}
