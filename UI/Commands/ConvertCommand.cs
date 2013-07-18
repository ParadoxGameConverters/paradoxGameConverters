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
            bool converterExists = File.Exists(this.Options.Converter);
            bool configurationFileExists = File.Exists(Path.Combine(Path.GetDirectoryName(this.Options.Converter), configurationFileName));
            bool saveGameExists = File.Exists(this.Options.SourceSaveGame);

            if (converterExists && configurationFileExists && saveGameExists)
            {
                return true;
            }
            
            return false;
        }

        protected override void OnExecute(object parameter)
        {
            using (var process = new Process())
            {
                process.StartInfo = new ProcessStartInfo
                {
                    FileName = this.Options.Converter,
                    Arguments = "\"" + Path.GetDirectoryName(this.Options.SourceSaveGame) + "\\" + Path.GetFileName(this.Options.SourceSaveGame) + "\"",
                    CreateNoWindow = true,
                    RedirectStandardError = true,
                    RedirectStandardInput = true,
                    UseShellExecute = false,
                    RedirectStandardOutput = true
                };

                process.OutputDataReceived += (sendingProcess, outLine) => this.Log(outLine.Data, LogEntrySeverity.Info, LogEntrySource.Converter);
                process.ErrorDataReceived += (sendingProcess, outLine) => this.Log(outLine.Data, LogEntrySeverity.Error, LogEntrySource.Converter);
                process.Exited += this.process_Exited;

                this.Log("Converting - this may take a few minutes...", LogEntrySeverity.Info, LogEntrySource.UI);
                Thread.Sleep(100);

                this.Stopwatch.Restart();
                process.EnableRaisingEvents = true;
                process.Start();

                process.BeginOutputReadLine();
                process.BeginErrorReadLine();
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
