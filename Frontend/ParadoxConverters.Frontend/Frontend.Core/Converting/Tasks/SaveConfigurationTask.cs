using Caliburn.Micro;
using Frontend.Core.Converting.Operations;
using Frontend.Core.Helpers;
using Frontend.Core.Logging;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.Proxies;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Converting.Tasks
{
    public class SaveConfigurationTask : ConverterTaskBase
    {
        private IDirectoryHelper directoryHelper;
        private IFileProxy fileProxy;
        private IFolderProxy folderProxy;
        private IOutputConfigurationFileHelper outputConfigurationHelper;
        private const string configurationFileName = "configuration.txt";

        public SaveConfigurationTask(IEventAggregator eventAggregator, 
            IConverterOptions options, 
            IDirectoryHelper directoryHelper, 
            IFileProxy fileProxy,
            IFolderProxy folderProxy,
            IOutputConfigurationFileHelper outputConfigurationHelper)
            : base(eventAggregator, options)
        {
            this.directoryHelper = directoryHelper;
            this.fileProxy = fileProxy;
            this.folderProxy = folderProxy;
            this.outputConfigurationHelper = outputConfigurationHelper;
        }

        public override string FriendlyName
        {
            get
            {
                return "Save configuration";
            }
        }

        protected override Task OnExecute(object parameter)
        {
            return new Task(() => this.SaveConfigurationFile());
        }

        /// <summary>
        /// Save the configuration file to disk
        /// </summary>
        private void SaveConfigurationFile()
        {
            var logEntries = new OperationResult();

            // Save configuration file to same directory as the converter itself lives in, wherever that might be
            var converterPathMinusFileName = this.directoryHelper.GetConverterWorkingDirectory(this.Options.CurrentConverter);
            var absoluteConfigurationFilePath = this.folderProxy.Combine(converterPathMinusFileName, configurationFileName);

            try
            {
                this.fileProxy.WriteAllText(absoluteConfigurationFilePath, this.outputConfigurationHelper.BuiltOutputString(this.Options.CurrentConverter, new DirectoryHelper())); //TODO: Consider encoding problems

                logEntries.LogEntries.Add(new LogEntry("Configuration file saved successfully as ", LogEntrySeverity.Info, LogEntrySource.UI, absoluteConfigurationFilePath));
            }
            catch (Exception e)
            {
                // No permitted to write to folder, ask user for elevated permission
                logEntries.LogEntries.Add(new LogEntry("Failed to save configuration file", LogEntrySeverity.Error, LogEntrySource.UI));
                logEntries.LogEntries.Add(new LogEntry("It may help running this application with administrator permissions", LogEntrySeverity.Error, LogEntrySource.UI));
                logEntries.LogEntries.Add(new LogEntry("Internal save error was: " + e.Message, LogEntrySeverity.Error, LogEntrySource.UI));
            }
        }
    }
}
