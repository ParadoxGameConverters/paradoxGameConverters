using Caliburn.Micro;
using Frontend.Core.Common.Proxies;
using Frontend.Core.Converting.Operations;
using Frontend.Core.Helpers;
using Frontend.Core.Logging;
using Frontend.Core.Model.Interfaces;
using System;
using System.Linq;
using System.Windows;

namespace Frontend.Core.Converting.Operations.CopyMod
{
    public class ModCopier : IModCopier
    {
        private IEventAggregator eventAggregator;
        private IConverterOptions options;
        private IFileProxy fileProxy;
        private IFolderProxy folderProxy;
        private IDirectoryHelper directoryHelper;
        private IMessageBoxProxy messageBoxProxy;
        private IDirectoryCopyHelper directoryCopyHelper;
        private ISaveGameNameTranslator nameTranslator;

        public ModCopier(
            IEventAggregator eventAggregator, 
            IConverterOptions options, 
            IFileProxy fileProxy, 
            IFolderProxy folderProxy, 
            IDirectoryHelper directoryHelper, 
            IMessageBoxProxy messageBoxProxy, 
            IDirectoryCopyHelper directoryCopyHelper, 
            ISaveGameNameTranslator nameTranslator)
        {
            this.eventAggregator = eventAggregator;
            this.options = options;
            this.fileProxy = fileProxy;
            this.folderProxy = folderProxy;
            this.directoryHelper = directoryHelper;
            this.messageBoxProxy = messageBoxProxy;
            this.directoryCopyHelper = directoryCopyHelper;
            this.nameTranslator = nameTranslator;
        }

        public OperationResult MoveModFileAndFolder()
        {
            var operationResult = new OperationResult();

            // NOTE: This method may be V2 specific. I'll have to talk to Idhrendur about the rules for how this is/will be handled in other converters. 
            // I'll figure out some generic way of handling any problems related to that when and if it occurs. 

            var targetGameModPathItem = options.CurrentConverter.RequiredItems.First(i => i.InternalTagName.Equals("targetGameModPath"));
            var desiredFileName = this.folderProxy.GetFileNameWithoutExtension(this.options.CurrentConverter.AbsoluteSourceSaveGame.SelectedValue) + this.options.CurrentConverter.TargetGame.SaveGameExtension;

            var translatedSaveGameName = this.nameTranslator.TranslateName(desiredFileName);

            // Copy the newly created output mod to the target game mod directory. 
            // The mod consists of two things: One file and one folder named after the save. Ex: The folder "France144_11_11" and the file "France144_11_11.mod".
            var converterWorkingDirectory = this.directoryHelper.GetConverterWorkingDirectory(this.options.CurrentConverter);
            var outputModFolderSourcePath = this.folderProxy.Combine(converterWorkingDirectory, "output", translatedSaveGameName);
            var outputModFileSourcePath = this.folderProxy.Combine(converterWorkingDirectory, "output", (translatedSaveGameName + ".mod"));

            var expectedAbsoluteOutputModFolderTargetPath = this.folderProxy.Combine(targetGameModPathItem.SelectedValue, translatedSaveGameName);
            var expectedAbsoluteOutputModFileTargetPath = expectedAbsoluteOutputModFolderTargetPath + ".mod";

            var modFileExists = this.fileProxy.Exists(expectedAbsoluteOutputModFileTargetPath);
            var modFolderExists = this.folderProxy.Exists(expectedAbsoluteOutputModFolderTargetPath);

            if (modFileExists || modFolderExists)
            {
                var confirmationMessage = string.Format("One or more parts of the output mod exists in {0} already. Overwrite?", targetGameModPathItem.SelectedValue);
                var result = this.messageBoxProxy.Show(confirmationMessage, "Confirmation Required", MessageBoxButton.YesNo);

                if (result == MessageBoxResult.No)
                {
                    operationResult.State = OperationResultState.Warning;
                    return operationResult;
                }
            }

            try
            {
                this.fileProxy.Copy(outputModFileSourcePath, expectedAbsoluteOutputModFileTargetPath, true);
                this.directoryCopyHelper.DirectoryCopy(outputModFolderSourcePath, expectedAbsoluteOutputModFolderTargetPath, true, true);

                operationResult.LogEntries.Add(new LogEntry("Mod copied to: ", LogEntrySeverity.Info, LogEntrySource.UI, expectedAbsoluteOutputModFolderTargetPath));
            }
            catch (Exception e)
            {
                operationResult.LogEntries.Add(new LogEntry(e.Message, LogEntrySeverity.Error, LogEntrySource.UI));
                operationResult.State = OperationResultState.Error;
            }

            return operationResult;
        }

    }
}
