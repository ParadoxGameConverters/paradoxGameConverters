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
        private Stopwatch sw;

        public ConvertCommand(ConverterOptions options)
            : base(options)
        {
        }

        private Stopwatch Stopwatch
        {
            get
            {
                return this.sw ?? (this.sw = new Stopwatch());
            }
        }

        protected override bool OnCanExecute(object parameter)
        {
            var outputPath = Path.GetDirectoryName(this.Options.Converter);

            if (outputPath == null || this.Options.SourceSaveGame == null)
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
                process.Exited += new EventHandler(this.process_Exited);

                this.Log("Converting - this may take a few minutes...", LogEntrySeverity.Info, LogEntrySource.UI);
                Thread.Sleep(100);

                this.Stopwatch.Restart();
                process.Start();

                process.BeginOutputReadLine();
                process.BeginErrorReadLine();

                //int timeout = 1000000;

                //if (process.WaitForExit(timeout))
                //{
                //}
                //else
                //{
                //    process.Kill();
                //    var t = TimeSpan.FromMilliseconds(timeout);
                //    this.Log("Conversion timed out after " + this.BuildTimeSpanString(t), LogEntrySeverity.Error, LogEntrySource.UI);
                //}
            }
        }

        private void process_Exited(object sender, EventArgs e)
        {
            var process = sender as Process;
            this.Stopwatch.Stop();

            if (process.ExitCode == 0)
            {
                this.Log("Conversion complete after " + this.BuildTimeSpanString(this.Stopwatch.Elapsed), LogEntrySeverity.Info, LogEntrySource.UI);
            }
            else
            {
                this.Log("Conversion failed after" + this.BuildTimeSpanString(this.Stopwatch.Elapsed), LogEntrySeverity.Error, LogEntrySource.UI);
            }

            process.CancelOutputRead();
            process.CancelErrorRead();
        }

        private string BuildTimeSpanString(TimeSpan timespan)
        {
            return string.Format("{0:D2}h:{1:D2}m:{2:D2}s:{3:D3}ms", timespan.Hours, timespan.Minutes, timespan.Seconds, timespan.Milliseconds);
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
