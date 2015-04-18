using Frontend.Core.Common.Proxies;
using Frontend.Core.Helpers;
using Frontend.Core.Logging;
using Frontend.Core.Model.Interfaces;
using System;
using System.Threading.Tasks;

namespace Frontend.Core.Converting.Operations.SaveConfiguration
{
    public class SaveConfigurationOperation : IOperation
    {
        private IDirectoryHelper directoryHelper;
        private IFileProxy fileProxy;
        private IFolderProxy folderProxy;
        private IOutputConfigurationFileHelper outputConfigurationHelper;
        private IConverterOptions options;
        private const string configurationFileName = "configuration.txt";

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
            get
            {
                return "Saving configuration...";
            }
        }

        public Task<OperationResult> Process()
        {
            return this.SaveConfiguration();;
        }

        public bool CanRun()
        {
            return fileProxy.Exists(this.options.CurrentConverter.AbsoluteSourceSaveGame.SelectedValue);
        }

        private async Task<OperationResult> SaveConfiguration()
        {
            var result = new OperationResult();
            var task = Task.FromResult<OperationResult>(result);

            var converterPathMinusFileName = this.directoryHelper.GetConverterWorkingDirectory(this.options.CurrentConverter);
            var absoluteConfigurationFilePath = this.folderProxy.Combine(converterPathMinusFileName, configurationFileName);

            try
            {
                this.fileProxy.WriteAllText(absoluteConfigurationFilePath, this.outputConfigurationHelper.BuiltOutputString(this.options.CurrentConverter, new DirectoryHelper())); //TODO: Consider encoding problems

                task.Result.LogEntries.Add(new LogEntry("Configuration file saved successfully as ", LogEntrySeverity.Info, LogEntrySource.UI, absoluteConfigurationFilePath));
            }
            catch (Exception e)
            {
                task.Result.State = OperationResultState.Error;
                // No permitted to write to folder, ask user for elevated permission
                task.Result.LogEntries.Add(new LogEntry("Failed to save configuration file", LogEntrySeverity.Error, LogEntrySource.UI));
                task.Result.LogEntries.Add(new LogEntry("It may help running this application with administrator permissions", LogEntrySeverity.Error, LogEntrySource.UI));
                task.Result.LogEntries.Add(new LogEntry("Internal save error was: " + e.Message, LogEntrySeverity.Error, LogEntrySource.UI));
            }

            return task.Result;
        }
    }
}
