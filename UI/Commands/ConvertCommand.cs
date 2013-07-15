using System.IO;
using Converter.UI.Settings;
using System.Diagnostics;
using System;
using Converter.UI.Enums;
using System.Windows.Threading;
using System.Windows;
using System.Threading;

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

            this.Log("Savegame (" + this.Options.SourceSaveGame + ") has been copied to " + destination + " and as input.ck2", LogEntrySeverity.Info, LogEntrySource.UI);

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

            using (var process = new Process())
            {
                process.StartInfo = new ProcessStartInfo
                {
                    FileName = this.Options.Converter,
                    CreateNoWindow = true,
                    RedirectStandardError = true,
                    RedirectStandardInput = true,
                    UseShellExecute = false, // According to http://msdn.microsoft.com/en-us/library/system.diagnostics.process.standardoutput.aspx
                    RedirectStandardOutput = true // these two properties must be set to true to capture any standard output from the converter.
                };

                process.OutputDataReceived += (sendingProcess, outLine) => this.Log(outLine.Data, LogEntrySeverity.Info, LogEntrySource.Converter);
                process.ErrorDataReceived += (sendingProcess, outLine) => this.Log(outLine.Data, LogEntrySeverity.Error, LogEntrySource.Converter);
                process.Exited += new EventHandler(process_Exited);

                this.Log("Starting conversion...", LogEntrySeverity.Info, LogEntrySource.UI);
                //Thread.Sleep(100);

                process.Start(); 
                Thread.Sleep(100);

                process.BeginOutputReadLine();
                process.BeginErrorReadLine();

                int timeout = 100000;

                if (process.WaitForExit(timeout))
                {
                }
                else
                {
                    process.Kill();
                    var t = TimeSpan.FromMilliseconds(timeout);
                    this.Log("Conversion timed out after " + string.Format("{0:D2}h:{1:D2}m:{2:D2}s:{3:D3}ms", t.Hours, t.Minutes, t.Seconds, t.Milliseconds), LogEntrySeverity.Error, LogEntrySource.UI);
                }

                

                //var result = process.StandardOutput.ReadToEnd();

                //while (!process.StandardOutput.EndOfStream)
                //{
                //    this.Options.Logger.AddLogEntry(new LogEntry(process.StandardOutput.BeginReadLine(), LogEntrySeverity.Info));
                //}
            }
        }

        private void process_Exited(object sender, EventArgs e)
        {
            var process = sender as Process;

            if (process.ExitCode == 0)
            {
                this.Log("Conversion complete", LogEntrySeverity.Info, LogEntrySource.UI);
            }
            else
            {
                this.Log("Conversion failed", LogEntrySeverity.Error, LogEntrySource.UI);
            }
        }

        private void Log(string text, LogEntrySeverity severity, LogEntrySource source)
        {
            if (String.IsNullOrEmpty(text))
            {
                return;
            }

            this.MarshallMethod(() =>
                {
                    this.Options.Logger.AddLogEntry(new LogEntry(text, severity, source));
                }, DispatcherPriority.Send);
        }
    }
}
