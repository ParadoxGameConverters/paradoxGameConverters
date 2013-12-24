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
using Converter.UI.Helpers;
using System.Text;

namespace Converter.UI.Commands
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
        public ConvertCommand(ConverterOptions options)
            : base(options)
        {
        }

        /////// <summary>
        /////// Gets the properties automatic monitor.
        /////// </summary>
        /////// <value>
        /////// The properties automatic monitor.
        /////// </value>
        ////private IList<string> PropertiesToMonitor 
        ////{
        ////    get
        ////    {
        ////        return this.propertiesToMonitor ?? (this.propertiesToMonitor = new List<string>() { "Converter", "SourceSaveGame" });
        ////    }
        ////}

        /// <summary>
        /// Called when [can execute].
        /// </summary>
        /// <param name="parameter">The parameter.</param>
        /// <returns></returns>
        protected override bool OnCanExecute(object parameter)
        {
            if (this.Options.Converter == null)
            {
                return false;
            }

            // Check various requirements
            bool converterExists = File.Exists(this.Options.Converter);
            bool configurationFileExists = File.Exists(Path.Combine(Path.GetDirectoryName(this.Options.Converter), configurationFileName));
            bool saveGameExists = File.Exists(this.Options.SourceSaveGame);

            if (converterExists && configurationFileExists && saveGameExists)
            {
                return true;
            }
            
            return false;
        }

        //protected override void BeforeExecute(object parameter)
        //{
        //    // Copy configuration files to working directory.

        //    if (!this.Options.UseConverterMod)
        //    {
        //        return;
        //        //TODO: Add code to reverse this as well
        //    }

        //    // Copy configuration files to working folder

        //    var conflicts = new List<string>();

        //    foreach (var file in this.Options.ModFilesProvider.ConfigurationFiles)
        //    {
        //        var path = Path.Combine(Environment.CurrentDirectory, file.FileName);

        //        if (File.Exists(path))
        //        {
        //            conflicts.Add(path);
        //        }
        //    }
        //}

        /// <summary>
        /// When overridden in a derived class, performs operations in a background
        /// thread when the <c>Execute</c> method is invoked.
        /// </summary>
        /// <param name="parameter">The paramter passed to the <c>Execute</c> method of the command.</param>
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
                Thread.Sleep(100); // Sleeping may let the UI actually display the above message before starting the conversion process

                Stopwatch stopwatch = new Stopwatch();
                stopwatch.Start();

                process.Start();
                
                // A non-working attempt to actually log the converter output while converting. 
                // As of this writing, I don't know if this fails due to the converter or something on the frontend side - it could be both.
                while (!process.StandardOutput.EndOfStream)
                {
                    this.Log(process.StandardOutput.ReadLine(), LogEntrySeverity.Info, LogEntrySource.Converter);
                }

                process.WaitForExit();

                stopwatch.Stop();

                if (process.ExitCode == 0)
                {
                    this.Options.WasConversionSuccessful = true;
                    this.Log("Conversion complete after " + this.BuildTimeSpanString(stopwatch.Elapsed), LogEntrySeverity.Info, LogEntrySource.UI);
                }
                else
                {
                    this.Options.WasConversionSuccessful = false;
                    this.Log("Conversion failed after" + this.BuildTimeSpanString(stopwatch.Elapsed), LogEntrySeverity.Error, LogEntrySource.UI);
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
            }
            else
            {
                StringBuilder sb = new StringBuilder();
                sb.Append("The converter crashed.");

                if (!this.Options.SourceGame.CurrentMod.IsDummyItem)
                {
                    sb.Append(" You configured the converter to use the mod \"" + this.Options.SourceGame.CurrentMod.Name + "\" - are you SURE the game was saved using this mod?");
                }

                this.Log(sb.ToString(), LogEntrySeverity.Error, LogEntrySource.UI);

                var log = Path.Combine(Environment.CurrentDirectory, "log.txt");

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
        /// Called when [successful conversion].
        /// </summary>
        private void OnSuccessfulConversion()
        {
            if (this.Options.UseConverterMod)
            {
                this.InstallConverterMod();
            }
            else
            {
                this.MoveSaveGame();
            }
        }

        /// <summary>
        /// Installs the converter mod.
        /// </summary>
        private void InstallConverterMod()
        {
            //var modFolder = this.Options.ModFilesProvider.ModFolder;

            var absoluteSourcePath = Path.Combine(Environment.CurrentDirectory, "Mod");

            // Verify source data
            if (!Directory.Exists(absoluteSourcePath))
            {
                // Log error
                this.Log("Could not find mod folder (" + absoluteSourcePath + "), making it somewhat hard to copy said folder to " + this.Options.TargetGame.InstallationPath + "\\mod" + ". This means that the converter mod isn't installed, so you might want to try to do it manually instead.", LogEntrySeverity.Error, LogEntrySource.UI);
                return;
            }

            // Overwrite needed?
            if (!File.Exists(this.Options.TargetGame.InstallationPath + "\\mod"))
            {
                // Ask permission for overwrite (or delete whole thing, then recopy?)
            }

            try
            {
                DirectoryCopyHelper.DirectoryCopy(absoluteSourcePath, this.Options.TargetGame.InstallationPath + "\\mod", true, true);
                this.Log("Converter mod copied successfully from " + absoluteSourcePath + " to " + this.Options.TargetGame.InstallationPath + "\\mod", LogEntrySeverity.Info, LogEntrySource.UI);
            }
            catch (Exception ex)
            {
                this.Log("Converter mod installation failed. Directory could not be copied from " + absoluteSourcePath + " to " + this.Options.TargetGame.InstallationPath + "\\mod" + ". The internal error message was: " + ex.Message, LogEntrySeverity.Error, LogEntrySource.UI);
            }
        }

        /// <summary>
        /// Moves the save game.
        /// </summary>
        private bool MoveSaveGame()
        {
            bool wasMoveSuccessful = false;

            // Copy the newly created save to the target game output directory.
            var desiredFileName = Path.GetFileNameWithoutExtension(this.Options.SourceSaveGame) + this.Options.TargetGame.SaveGameExtension;
            var canOverWrite = false;
            var expectedOutputDirectoryAndFile = Path.Combine(this.Options.TargetGame.InstallationPath + this.Options.TargetGame.SaveGamePath, desiredFileName);

            // Don't blindly overwrite any existing saves - that's just rude
            if (File.Exists(expectedOutputDirectoryAndFile))
            {
                var overwriteQuestion = "A " + this.Options.TargetGame.FriendlyName + " save with the same name (" + desiredFileName + ") exists already. " + Environment.NewLine + Environment.NewLine
                    + "Do you want to overwrite this file? " + Environment.NewLine + "Full path: " + expectedOutputDirectoryAndFile;
                var result = MessageBox.Show(overwriteQuestion, "Confirmation Required", MessageBoxButton.YesNo);

                if (result == MessageBoxResult.No)
                {
                    this.Log("The file \"" + expectedOutputDirectoryAndFile + "\" existed already, and was not replaced. You should copy \"" + this.DetermineOutputSavePath()
                        + "\" to \"" + this.Options.TargetGame.SaveGamePath + "\" to load the converted save.", LogEntrySeverity.Warning, LogEntrySource.UI);
                    return false;
                }

                // Permission granted, continue
                canOverWrite = true;
            }

            try
            {
                var outputSavePath = this.DetermineOutputSavePath();
                File.Copy(outputSavePath, expectedOutputDirectoryAndFile, canOverWrite);
                this.Log(desiredFileName + " has been written to \"" + this.Options.TargetGame.InstallationPath + this.Options.TargetGame.SaveGamePath + "\".", LogEntrySeverity.Info, LogEntrySource.UI);

                //File.Delete(outputSavePath);
                //this.Log("Deleted temporary file(s).", LogEntrySeverity.Info, LogEntrySource.UI);
                wasMoveSuccessful = true;
            }
            catch (Exception e)
            {
                this.Log(e.Message, LogEntrySeverity.Error, LogEntrySource.UI);
                wasMoveSuccessful = false;
            }

            return wasMoveSuccessful;
        }

        /// <summary>
        /// Determines the output save path.
        /// </summary>
        /// <returns></returns>
        private string DetermineOutputSavePath()
        {
            return Path.GetFileNameWithoutExtension(this.Options.SourceSaveGame) + this.Options.TargetGame.SaveGameExtension;
        }

        /// <summary>
        /// Builds the time span string.
        /// </summary>
        /// <param name="timespan">The timespan.</param>
        /// <returns></returns>
        private string BuildTimeSpanString(TimeSpan timespan)
        {
            return string.Format("{1:D2}m:{2:D2}s:{3:D3}ms", timespan.Hours, timespan.Minutes, timespan.Seconds, timespan.Milliseconds);
        }

        /// <summary>
        /// Logs the specified text. This is marshalled to the UI thread if necessary.
        /// </summary>
        /// <param name="text">The text.</param>
        /// <param name="severity">The severity.</param>
        /// <param name="source">The source.</param>
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
