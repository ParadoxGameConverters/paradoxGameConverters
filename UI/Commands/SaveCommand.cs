

namespace Converter.UI.Commands
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
using Converter.UI.Settings;
    using System.IO;
    using Converter.UI.Helpers;
    using System.Windows;
    using Converter.UI.Enums;

    public class SaveCommand : CommandBase
    {
        private const string configurationFileName = "configuration.txt";

        public SaveCommand(ConverterOptions options)
            : base(options)
        {
        }

        protected override bool OnCanExecute(object parameter)
        {
            //TODO: Validate options object
            //NOTE: Careful not to judge based on incorrect data, esp. if user has edited the configuration output manually
            return true;
        }

        protected override void OnExecute(object parameter)
        {
            // Reset configuration setting to make sure it's updated with the latest user choices.
            this.Options.InvalidateOutputConfiguration();

            var outputPath = Path.GetDirectoryName(this.Options.Converter);

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
                var combinedPath = Path.Combine(outputPath, configurationFileName);

                // Check permissions
                if (true)//DiskPermissionHelper.CanWriteFileToFolder(outputPath))
                {
                    File.WriteAllText(combinedPath, this.Options.OutputConfiguration); //TODO: Consider encoding problems
                    this.Options.Logger.AddLogEntry(new LogEntry("Configuration file saved successfully", LogEntrySeverity.Info, LogEntrySource.UI));
                }
                else
                {
                }
            }
            catch (Exception e)
            {
                // No permitted to write to folder, ask user for elevated permission
                this.Options.Logger.AddLogEntry(new LogEntry("Failed to save configuration file", LogEntrySeverity.Error, LogEntrySource.UI));
                this.Options.Logger.AddLogEntry(new LogEntry("Try running this application with administrator permissions", LogEntrySeverity.Error, LogEntrySource.UI));

                //MessageBox.Show("Failed to save configuration file. The error given by the OS was: " + e.Message, 
                //    "Error saving configuration", 
                //    MessageBoxButton.OK, 
                //    MessageBoxImage.Error);
            }
        }
    }
}
