using Caliburn.Micro;
using Frontend.Core.Helpers;
using Frontend.Core.Logging;
using Frontend.Core.Model.Interfaces;
using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows;
using System.Windows.Threading;

namespace Frontend.Core.Commands
{
    /// <summary>
    /// The command that triggers the conversion process.
    /// </summary>
    public class ConvertCommand : AsyncCommandBase
    {
        private const string configurationFileName = "configuration.txt";

        /// <summary>
        /// Initializes a new instance of the <see cref="ConvertCommand"/> class.
        /// </summary>
        /// <param name="options">The options.</param>
        public ConvertCommand(IEventAggregator eventAggregator, IConverterOptions options)
            : base(eventAggregator, options)
        {
        }

        /// <summary>
        /// Called when [can execute].
        /// </summary>
        /// <param name="parameter">The parameter.</param>
        /// <returns></returns>
        protected override bool OnCanExecute(object parameter)
        {
            // Check various requirements
            var saveGame = this.Options.CurrentConverter.RequiredItems.First(i => i.TagName.Equals("SaveGame"));
            
            if (saveGame == null || !File.Exists(saveGame.SelectedValue))
            {
                return false;
            }

            if (File.Exists(this.Options.CurrentConverter.AbsoluteConverter.SelectedValue))
            {
                return true;
            }

            return false;
        }

        /// <summary>
        /// When overridden in a derived class, performs operations in a background
        /// thread when the <c>Execute</c> method is invoked.
        /// </summary>
        /// <param name="parameter">The paramter passed to the <c>Execute</c> method of the command.</param>
        protected override void OnExecute(object parameter)
        {
            this.SaveConfigurationFile();

            // Reading process output syncronously. The async part is already handled by the command
            using (var process = new Process())
            {
                var argument = "\"" + this.Options.CurrentConverter.AbsoluteSourceSaveGame.SelectedValue + "\"";

                process.StartInfo = new ProcessStartInfo
                {
                    FileName = this.Options.CurrentConverter.AbsoluteConverter.SelectedValue,
                    Arguments = argument,
                    CreateNoWindow = true,
                    RedirectStandardError = true,
                    RedirectStandardInput = true,
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    WorkingDirectory = DirectoryHelper.GetConverterWorkingDirectory(this.Options.CurrentConverter)
                };

                this.Log("Converting - this may take a few minutes...", LogEntrySeverity.Info, LogEntrySource.UI, null);
                Thread.Sleep(100); // Sleeping may let the UI actually display the above message before starting the conversion process

                Stopwatch stopwatch = new Stopwatch();
                stopwatch.Start();

                process.Start();

                // A non-working attempt to actually log the converter output while converting. 
                // As of this writing, I don't know if this fails due to the converter or something on the frontend side - it could be both.
                while (!process.StandardOutput.EndOfStream)
                {
                    this.Log(process.StandardOutput.ReadLine(), LogEntrySeverity.Info, LogEntrySource.Converter, null);
                }

                process.WaitForExit();

                stopwatch.Stop();

                if (process.ExitCode == 0)
                {
                    this.Options.WasConversionSuccessful = true;
                    this.Log("Conversion complete after " + this.BuildTimeSpanString(stopwatch.Elapsed), LogEntrySeverity.Info, LogEntrySource.UI, null);
                }
                else
                {
                    this.Options.WasConversionSuccessful = false;
                    this.Log("Conversion failed after" + this.BuildTimeSpanString(stopwatch.Elapsed), LogEntrySeverity.Error, LogEntrySource.UI, null);
                }
            }
        }

        /// <summary>
        /// When overridden in a derived class, performs operations when the background execution has completed.
        /// </summary>
        /// <param name="parameter">The parameter passed to the <c>Execute</c> method of the command.</param>
        /// <param name="error">The error object that was thrown during the background operation, or null if no error was thrown.</param>
        protected override void AfterExecute(object parameter, Exception error)
        {
            if (this.Options.WasConversionSuccessful)
            {
                this.OnSuccessfulConversion();
                //this.EventAggregator.PublishOnUIThread(new LogEntry("Conversion successful!", LogEntrySeverity.Info, LogEntrySource.UI));
            }
            else
            {
                StringBuilder sb = new StringBuilder();
                sb.Append("The converter crashed.");

                //if (!this.Options.SourceGame.CurrentMod.IsDummyItem)
                //{
                //    sb.Append(" You configured the converter to use the mod \"" + this.Options.SourceGame.CurrentMod.Name + "\" - are you SURE the game was saved using this mod?");
                //}

                this.Log(sb.ToString(), LogEntrySeverity.Error, LogEntrySource.UI, null);

                var log = Path.Combine(DirectoryHelper.GetConverterWorkingDirectory(this.Options.CurrentConverter), "log.txt");

                if (File.Exists(log))
                {
                    var result = MessageBox.Show("The conversion failed. Do you want to show the internal conversion log?", "Conversion log", MessageBoxButton.YesNo, MessageBoxImage.Question, MessageBoxResult.Yes);

                    if (result == MessageBoxResult.Yes)
                    {
                        Process.Start(log);
                    }
                }
            }
        }

        /// <summary>
        /// Save the configuration file to disk
        /// </summary>
        private void SaveConfigurationFile()
        {
            // First, save configuration file
            try
            {
                // Save configuration file to same directory as the converter itself lives in, wherever that might be
                var converterPathMinusFileName = DirectoryHelper.GetConverterWorkingDirectory(this.Options.CurrentConverter);
                var absoluteConfigurationFilePath = Path.Combine(converterPathMinusFileName, configurationFileName);

                if (true)//DiskPermissionHelper.CanWriteFileToFolder(outputPath))
                {
                    File.WriteAllText(absoluteConfigurationFilePath, OutputConfigurationFileHelper.BuiltOutputString(this.Options.CurrentConverter)); //TODO: Consider encoding problems
                    this.EventAggregator.PublishOnUIThread(new LogEntry("Configuration file saved successfully as ", LogEntrySeverity.Info, LogEntrySource.UI, absoluteConfigurationFilePath));
                }
            }
            catch (Exception e)
            {
                // No permitted to write to folder, ask user for elevated permission
                this.EventAggregator.PublishOnUIThread(new LogEntry("Failed to save configuration file", LogEntrySeverity.Error, LogEntrySource.UI));
                this.EventAggregator.PublishOnUIThread(new LogEntry("It may help running this application with administrator permissions", LogEntrySeverity.Error, LogEntrySource.UI));
                this.EventAggregator.PublishOnUIThread(new LogEntry("Internal save error was: " + e.Message, LogEntrySeverity.Error, LogEntrySource.UI));
            }
        }

        /// <summary>
        /// Called when [successful conversion].
        /// </summary>
        private void OnSuccessfulConversion()
        {
            if (this.Options.CurrentConverter.UseConverterMod)
            {
                this.InstallConverterMod();
            }
            else
            {
                this.MoveOutputMod();
            }

            //TODO: Do we ever need to do both?
        }

        /// <summary>
        /// Installs the converter mod.
        /// </summary>
        private void InstallConverterMod()
        {
            throw new NotImplementedException();
        }
        
        /// <summary>
        /// Moves the output mod file and folders
        /// </summary>
        private void MoveOutputMod()
        {
            // NOTE: This method may be V2 specific. I'll have to talk to Idhrendur about the rules for how this is/will be handled in other converters. 
            // I'll figure out some generic way of handling any problems related to that when and if it occurs. 

            var targetGameModPathItem = this.Options.CurrentConverter.RequiredItems.First(i => i.InternalTagName.Equals("targetGameModPath"));
            var desiredFileName = Path.GetFileNameWithoutExtension(this.Options.CurrentConverter.AbsoluteSourceSaveGame.SelectedValue) + this.Options.CurrentConverter.TargetGame.SaveGameExtension;

            // Copy the newly created output mod to the target game mod directory. 
            // The mod consists of two things: One file and one folder named after the save. Ex: The folder "France144_11_11" and the file "France144_11_11.mod".
            var converterWorkingDirectory = DirectoryHelper.GetConverterWorkingDirectory(this.Options.CurrentConverter);
            var outputModFolderSourcePath = Path.Combine(converterWorkingDirectory, "output", desiredFileName);
            var outputModFileSourcePath = Path.Combine(converterWorkingDirectory, "output", (desiredFileName + ".mod"));

            var expectedAbsoluteOutputModFolderTargetPath = Path.Combine(targetGameModPathItem.SelectedValue, desiredFileName);
            var expectedAbsoluteOutputModFileTargetPath = expectedAbsoluteOutputModFolderTargetPath + ".mod";

            var modFileExists = File.Exists(expectedAbsoluteOutputModFileTargetPath);
            var modFolderExists = Directory.Exists(expectedAbsoluteOutputModFolderTargetPath);

            if (modFileExists || modFolderExists)
            {
                var confirmationMessage = string.Format("One or more parts of the output mod exists in {0} already. Overwrite?", targetGameModPathItem.SelectedValue);
                var result = MessageBox.Show(confirmationMessage, "Confirmation Required", MessageBoxButton.YesNo);

                if (result == MessageBoxResult.No)
                {
                    return;
                }
            }

            try
            {
                File.Copy(outputModFileSourcePath, expectedAbsoluteOutputModFileTargetPath, true);
                DirectoryCopyHelper.DirectoryCopy(outputModFolderSourcePath, expectedAbsoluteOutputModFolderTargetPath, true, true);
                this.EventAggregator.PublishOnUIThread(new LogEntry("Copy complete. Ready to play - have fun!", LogEntrySeverity.Info, LogEntrySource.UI));                
            }
            catch (Exception e)
            {
                this.Log(e.Message, LogEntrySeverity.Error, LogEntrySource.UI, null);
            }
        }


        /// <summary>
        /// Builds the time span string.
        /// </summary>
        /// <param name="timespan">The timespan.</param>
        /// <returns></returns>
        private string BuildTimeSpanString(TimeSpan timespan)
        {
            return TimeStringFormatter.BuildTimeString(timespan);
        }

        /// <summary>
        /// Logs the specified text. This is marshalled to the UI thread if necessary.
        /// </summary>
        /// <param name="text">The text.</param>
        /// <param name="severity">The severity.</param>
        /// <param name="source">The source.</param>
        private void Log(string text, LogEntrySeverity severity, LogEntrySource source, string path)
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
                    this.EventAggregator.PublishOnUIThread(new LogEntry(text, LogEntrySeverity.Error, LogEntrySource.Converter, path));
                    return;
                }

                this.EventAggregator.PublishOnUIThread(new LogEntry(text, severity, source, path));
            }, DispatcherPriority.Send);
        }
    }
}
