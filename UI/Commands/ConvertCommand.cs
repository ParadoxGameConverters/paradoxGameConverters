using System.IO;
using Converter.UI.Settings;
using System.Diagnostics;
using System;
using Converter.UI.Enums;
using System.Windows.Threading;

namespace Converter.UI.Commands
{
    public class ConvertCommand : CommandBase
    {
        private const string configurationFileName = "configuration.txt";

        public ConvertCommand(ConverterOptions options)
            : base(options)
        {
        }

        protected override bool OnCanExecute(object parameter)
        {
            var outputPath = Path.GetDirectoryName(this.Options.Converter);

            if (outputPath == null)
            {
                return false;
            }

            return File.Exists(Path.Combine(outputPath, configurationFileName));
        }

        protected override void OnExecute(object parameter)
        {
            // Step 1: Copy the selected savegame to the working directory
            var destination = Path.Combine(Path.GetDirectoryName(this.Options.Converter), "input.ck2");
            File.Copy(this.Options.SourceSaveGame, destination, true);

            // Step 2: run the converter
            //var converterProcessInfo = new ProcessStartInfo
            //{
            //    FileName = this.Options.Converter,
            //    UseShellExecute = false, // According to http://msdn.microsoft.com/en-us/library/system.diagnostics.process.standardoutput.aspx
            //    RedirectStandardOutput = true // these two properties must be set to true to capture any standard output from the converter.
            //};

            //using (Process process = Process.Start(converterProcessInfo))
            //{
            //    using (StreamReader reader = process.StandardOutput)
            //    {
            //        this.log
            //    }
            //}

            var process = new Process();

            process.StartInfo = new ProcessStartInfo
            {
                FileName = this.Options.Converter,
                CreateNoWindow = true,
                RedirectStandardError = true,
                RedirectStandardInput = true,
                UseShellExecute = false, // According to http://msdn.microsoft.com/en-us/library/system.diagnostics.process.standardoutput.aspx
                RedirectStandardOutput = true // these two properties must be set to true to capture any standard output from the converter.
            };

            process.ErrorDataReceived += new DataReceivedEventHandler(OnProcess_ErrorDataReceived);
            process.OutputDataReceived += new DataReceivedEventHandler(OnProcess_OutputDataReceived);

            this.Log("Starting conversion...", LogEntrySeverity.Info, LogEntrySource.UI);
            process.Start();
            process.BeginOutputReadLine();
            process.WaitForExit();
            process.Close();
            this.Log("Conversion complete", LogEntrySeverity.Info, LogEntrySource.UI);

            //var result = process.StandardOutput.ReadToEnd();

            //while (!process.StandardOutput.EndOfStream)
            //{
            //    this.Options.Logger.AddLogEntry(new LogEntry(process.StandardOutput.BeginReadLine(), LogEntrySeverity.Info));
            //}
        }

        private void OnProcess_OutputDataReceived(object sender, DataReceivedEventArgs e)
        {
            if (!String.IsNullOrEmpty(e.Data))
            {
                this.Log(e.Data, LogEntrySeverity.Info, LogEntrySource.Converter);
            }
        }

        private void OnProcess_ErrorDataReceived(object sender, DataReceivedEventArgs e)
        {
            if (!String.IsNullOrEmpty(e.Data))
            {
                this.Log(e.Data, LogEntrySeverity.Error, LogEntrySource.Converter);
            }
        }

        private void Log(string text, LogEntrySeverity severity, LogEntrySource source)
        {
            this.MarshallMethod(() =>
                {
                    this.Options.Logger.AddLogEntry(new LogEntry(text, severity, source));
                }, DispatcherPriority.Send);
        }
    }
}
