using Caliburn.Micro;
using Frontend.Core.Helpers;
using Frontend.Core.Logging;
using Frontend.Core.Model.Interfaces;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Commands
{
    /// <summary>
    /// This command is used to save the configuration.txt file to disk in the working directory
    /// </summary>
    public class SaveCommand : CommandBase
    {
        private const string configurationFileName = "configuration.txt";

        /// <summary>
        /// Initializes a new instance of the <see cref="SaveCommand"/> class.
        /// </summary>
        /// <param name="eventAggregator">The event aggregator</param>
        /// <param name="options">The options.</param>
        public SaveCommand(IEventAggregator eventAggregator, IConverterOptions options)
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
            //TODO: Validate options object
            return true;
        }

        /// <summary>
        /// Called when [execute].
        /// </summary>
        /// <param name="parameter">The parameter.</param>
        protected override void OnExecute(object parameter)
        {
            // Reset configuration setting to make sure it's updated with the latest user choices.
            //this.Options.InvalidateOutputConfiguration();

            //var outputPath = Path.GetDirectoryName(this.Options.Converter);

            //TODO:
            /* check permission. 
             * if allowed:
             * if file exists, ask to replace
             * if not, write
             * 
             * if not allowed:
             * ask to elevate?
             */

            try
            {
                // Save configuration file to same directory as the converter itself lives in, wherever that might be
                var converterPathMinusFileName = WorkingDirectoryHelper.GetConverterWorkingDirectory(this.Options.CurrentConverter);
                var absoluteConfigurationFilePath = Path.Combine(converterPathMinusFileName, configurationFileName);

                // Check permissions
                if (true)//DiskPermissionHelper.CanWriteFileToFolder(outputPath))
                {
                    File.WriteAllText(absoluteConfigurationFilePath, OutputConfigurationFileHelper.BuiltOutputString(this.Options.CurrentConverter)); //TODO: Consider encoding problems
                    this.EventAggregator.PublishOnUIThread(new LogEntry("Configuration file saved successfully as ", LogEntrySeverity.Info, LogEntrySource.UI, absoluteConfigurationFilePath));
                }
                else
                {
                }
            }
            catch (Exception e)
            {
                // No permitted to write to folder, ask user for elevated permission
                this.EventAggregator.PublishOnUIThread(new LogEntry("Failed to save configuration file", LogEntrySeverity.Error, LogEntrySource.UI));
                this.EventAggregator.PublishOnUIThread(new LogEntry("It may help running this application with administrator permissions", LogEntrySeverity.Error, LogEntrySource.UI));
                this.EventAggregator.PublishOnUIThread(new LogEntry("Internal save error was: " + e.Message, LogEntrySeverity.Error, LogEntrySource.UI));

                //MessageBox.Show("Failed to save configuration file. The error given by the OS was: " + e.Message, 
                //    "Error saving configuration", 
                //    MessageBoxButton.OK, 
                //    MessageBoxImage.Error);
            }
        }
    }
}
