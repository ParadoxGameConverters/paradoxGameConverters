using System;
using Frontend.Core.Common.Proxies;
using Frontend.Core.Helpers;
using Frontend.Core.Logging;
using Frontend.Core.Model.Interfaces;

namespace Frontend.Core.Converting.Operations.SaveConfiguration
{
    public class SaveConfigurationOperation : IOperation
    {
        private const string configurationFileName = "configuration.txt";
        private readonly IDirectoryHelper directoryHelper;
        private readonly IFileProxy fileProxy;
        private readonly IFolderProxy folderProxy;
        private readonly IConverterOptions options;
        private readonly IOutputConfigurationFileHelper outputConfigurationHelper;

        public SaveConfigurationOperation(
            IConverterOptions options,
            IDirectoryHelper directoryHelper,
            IFileProxy fileProxy,
            IFolderProxy folderProxy,
            IOutputConfigurationFileHelper outputConfigurationHelper)
        {
            this.options = options;
            this.directoryHelper = directoryHelper;
            this.fileProxy = fileProxy;
            this.folderProxy = folderProxy;
            this.outputConfigurationHelper = outputConfigurationHelper;
        }

        public string Description
        {
            get { return "Saving configuration..."; }
        }

        public OperationResult Process()
        {
            return SaveConfiguration();
            ;
        }

        public bool CanRun()
        {
            return fileProxy.Exists(options.CurrentConverter.AbsoluteSourceSaveGame.SelectedValue);
        }

        private OperationResult SaveConfiguration()
        {
            var result = new OperationResult();

            var converterPathMinusFileName = directoryHelper.GetConverterWorkingDirectory(options.CurrentConverter);
            var absoluteConfigurationFilePath = folderProxy.Combine(converterPathMinusFileName, configurationFileName);

            try
            {
                fileProxy.WriteAllText(absoluteConfigurationFilePath,
                    outputConfigurationHelper.BuiltOutputString(options.CurrentConverter, new DirectoryHelper()));
                    //TODO: Consider encoding problems

                result.LogEntries.Add(new LogEntry("Configuration file saved successfully as ", LogEntrySeverity.Info,
                    LogEntrySource.UI, absoluteConfigurationFilePath));
            }
            catch (Exception e)
            {
                result.State = OperationResultState.Error;
                // No permitted to write to folder, ask user for elevated permission
                result.LogEntries.Add(new LogEntry("Failed to save configuration file", LogEntrySeverity.Error,
                    LogEntrySource.UI));
                result.LogEntries.Add(new LogEntry(
                    "It may help running this application with administrator permissions", LogEntrySeverity.Error,
                    LogEntrySource.UI));
                result.LogEntries.Add(new LogEntry("Internal save error was: " + e.Message, LogEntrySeverity.Error,
                    LogEntrySource.UI));
            }

            return result;
        }
    }
}