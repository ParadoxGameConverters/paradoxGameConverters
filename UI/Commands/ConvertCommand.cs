using System.IO;
using Converter.UI.Settings;
using System.Diagnostics;
using System;
using Converter.UI.Enums;
using System.Windows.Threading;
using System.Windows;
using System.Threading;
using System.ComponentModel;
using System.Collections.Generic;

namespace Converter.UI.Commands
{
    public class ConvertCommand : AsyncCommandBase
    {
        private const string configurationFileName = "configuration.txt";
        private IList<string> propertiesToMonitor;

        public ConvertCommand(ConverterOptions options)
            : base(options)
        {
        }

        private IList<string> PropertiesToMonitor 
        {
            get
            {
                return this.propertiesToMonitor ?? (this.propertiesToMonitor = new List<string>() { "Converter", "SourceSaveGame" });
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
            // Reading process output syncronously. The async part is already handled by the command
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

                this.Log("Converting - this may take a few minutes...", LogEntrySeverity.Info, LogEntrySource.UI);
                Thread.Sleep(100);

                Stopwatch stopwatch = new Stopwatch();
                stopwatch.Start();

                process.Start();
                
                while (!process.StandardOutput.EndOfStream)
                {
                    this.Log(process.StandardOutput.ReadLine(), LogEntrySeverity.Info, LogEntrySource.Converter);
                }

                process.WaitForExit();

                stopwatch.Stop();

                if (process.ExitCode == 0)
                {
                    this.Log("Conversion complete after " + this.BuildTimeSpanString(stopwatch.Elapsed), LogEntrySeverity.Info, LogEntrySource.UI);
                }
                else
                {
                    this.Log("Conversion failed after" + this.BuildTimeSpanString(stopwatch.Elapsed), LogEntrySeverity.Error, LogEntrySource.UI);
                }
            }
        }

        private string BuildTimeSpanString(TimeSpan timespan)
        {
            return string.Format("{1:D2}m:{2:D2}s:{3:D3}ms", timespan.Hours, timespan.Minutes, timespan.Seconds, timespan.Milliseconds);
        }

        private void Log(string text, LogEntrySeverity severity, LogEntrySource source)
        {
            if (String.IsNullOrEmpty(text))
            {
                return;
            }

            this.MarshallMethod(() =>
                {
                    //Hack to catch error messages from the converter
                    if (text.StartsWith("Error: "))
                    {
                        this.Options.Logger.AddLogEntry(new LogEntry(text, LogEntrySeverity.Error, LogEntrySource.Converter));
                        return;
                    }

                    this.Options.Logger.AddLogEntry(new LogEntry(text, severity, source));
                }, DispatcherPriority.Send);
        }
    }
}
