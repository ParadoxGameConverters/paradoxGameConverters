using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows;
using System.Windows.Threading;
using Caliburn.Micro;
using Frontend.Core.Commands;
using Frontend.Core.Converting.Operations.CopyMod;
using Frontend.Core.Helpers;
using Frontend.Core.Logging;
using Frontend.Core.Model.Interfaces;

namespace Frontend.Core.Converting
{
    /// <summary>
    ///     The command that triggers the conversion process.
    /// </summary>
    public class OldConvertCommand : OldAsyncCommandBase
    {
        private readonly IDirectoryHelper directoryHelper;
        private readonly IModCopier modCopier;

        /// <summary>
        ///     Initializes a new instance of the <see cref="OldConvertCommand" /> class.
        /// </summary>
        /// <param name="options">The options.</param>
        public OldConvertCommand(IEventAggregator eventAggregator, IConverterOptions options, IModCopier modCopier,
            IDirectoryHelper directoryHelper)
            : base(eventAggregator, options)
        {
            this.modCopier = modCopier;
            this.directoryHelper = directoryHelper;
        }

        /// <summary>
        ///     Called when [can execute].
        /// </summary>
        /// <param name="parameter">The parameter.</param>
        /// <returns></returns>
        protected override bool OnCanExecute(object parameter)
        {
            // Check various requirements
            var saveGame = Options.CurrentConverter.RequiredItems.First(i => i.TagName.Equals("SaveGame"));

            if (saveGame == null || !File.Exists(saveGame.SelectedValue))
            {
                return false;
            }

            if (File.Exists(Options.CurrentConverter.AbsoluteConverter.SelectedValue))
            {
                return true;
            }

            return false;
        }

        /// <summary>
        ///     When overridden in a derived class, performs operations in a background
        ///     thread when the <c>Execute</c> method is invoked.
        /// </summary>
        /// <param name="parameter">The paramter passed to the <c>Execute</c> method of the command.</param>
        protected override void OnExecute(object parameter)
        {
            //this.MarshallMethod(() => this.SaveConfigurationFile(), DispatcherPriority.Send);

            // Reading process output syncronously. The async part is already handled by the command
            using (var process = new Process())
            {
                var argument = "\"" + Options.CurrentConverter.AbsoluteSourceSaveGame.SelectedValue + "\"";

                process.StartInfo = new ProcessStartInfo
                {
                    FileName = Options.CurrentConverter.AbsoluteConverter.SelectedValue,
                    Arguments = argument,
                    CreateNoWindow = true,
                    RedirectStandardError = true,
                    RedirectStandardInput = true,
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    WorkingDirectory = directoryHelper.GetConverterWorkingDirectory(Options.CurrentConverter)
                };

                Log("Converting - this may take a few minutes...", LogEntrySeverity.Info, LogEntrySource.UI, null);
                Thread.Sleep(100);
                    // Sleeping may let the UI actually display the above message before starting the conversion process

                var stopwatch = new Stopwatch();
                stopwatch.Start();

                process.Start();

                // A non-working attempt to actually log the converter output while converting. 
                // As of this writing, I don't know if this fails due to the converter or something on the frontend side - it could be both.
                while (!process.StandardOutput.EndOfStream)
                {
                    Log(process.StandardOutput.ReadLine(), LogEntrySeverity.Info, LogEntrySource.Converter, null);
                }

                process.WaitForExit();

                stopwatch.Stop();

                if (process.ExitCode == 0)
                {
                    Options.WasConversionSuccessful = true;
                    Log("Conversion complete after " + BuildTimeSpanString(stopwatch.Elapsed), LogEntrySeverity.Info,
                        LogEntrySource.UI, null);
                }
                else
                {
                    Options.WasConversionSuccessful = false;
                    Log("Conversion failed after" + BuildTimeSpanString(stopwatch.Elapsed), LogEntrySeverity.Error,
                        LogEntrySource.UI, null);
                }
            }
        }

        /// <summary>
        ///     When overridden in a derived class, performs operations when the background execution has completed.
        /// </summary>
        /// <param name="parameter">The parameter passed to the <c>Execute</c> method of the command.</param>
        /// <param name="error">The error object that was thrown during the background operation, or null if no error was thrown.</param>
        protected override void AfterExecute(object parameter, Exception error)
        {
            if (Options.WasConversionSuccessful)
            {
                OnSuccessfulConversion();
                //this.EventAggregator.PublishOnUIThread(new LogEntry("Conversion successful!", LogEntrySeverity.Info, LogEntrySource.UI));
            }
            else
            {
                var sb = new StringBuilder();
                sb.Append("The converter crashed.");

                //if (!this.Options.SourceGame.CurrentMod.IsDummyItem)
                //{
                //    sb.Append(" You configured the converter to use the mod \"" + this.Options.SourceGame.CurrentMod.Name + "\" - are you SURE the game was saved using this mod?");
                //}

                Log(sb.ToString(), LogEntrySeverity.Error, LogEntrySource.UI, null);

                var log = Path.Combine(directoryHelper.GetConverterWorkingDirectory(Options.CurrentConverter), "log.txt");

                if (File.Exists(log))
                {
                    var result =
                        MessageBox.Show("The conversion failed. Do you want to show the internal conversion log?",
                            "Conversion log", MessageBoxButton.YesNo, MessageBoxImage.Question, MessageBoxResult.Yes);

                    if (result == MessageBoxResult.Yes)
                    {
                        Process.Start(log);
                    }
                }
            }
        }

        /// <summary>
        ///     Called when [successful conversion].
        /// </summary>
        private void OnSuccessfulConversion()
        {
            if (Options.CurrentConverter.UseConverterMod)
            {
                InstallConverterMod();
            }
            else
            {
                modCopier.MoveModFileAndFolder();
            }

            //TODO: Do we ever need to do both?
        }

        /// <summary>
        ///     Installs the converter mod.
        /// </summary>
        private void InstallConverterMod()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        ///     Builds the time span string.
        /// </summary>
        /// <param name="timespan">The timespan.</param>
        /// <returns></returns>
        private string BuildTimeSpanString(TimeSpan timespan)
        {
            return TimeStringFormatter.BuildTimeString(timespan);
        }

        /// <summary>
        ///     Logs the specified text. This is marshalled to the UI thread if necessary.
        /// </summary>
        /// <param name="text">The text.</param>
        /// <param name="severity">The severity.</param>
        /// <param name="source">The source.</param>
        private void Log(string text, LogEntrySeverity severity, LogEntrySource source, string path)
        {
            if (string.IsNullOrEmpty(text))
            {
                return;
            }

            MarshallMethod(() =>
            {
                //Hack to catch error messages from the converter
                if (text.StartsWith("Error: "))
                {
                    EventAggregator.PublishOnUIThread(new LogEntry(text, LogEntrySeverity.Error,
                        LogEntrySource.Converter, path));
                    return;
                }

                EventAggregator.PublishOnUIThread(new LogEntry(text, severity, source, path));
            }, DispatcherPriority.Send);
        }
    }
}