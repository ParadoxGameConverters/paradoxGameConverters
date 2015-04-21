using System;
using System.Linq;
using System.Windows;
using Frontend.Core.Common.Proxies;
using Frontend.Core.Helpers;
using Frontend.Core.Logging;
using Frontend.Core.Model.Interfaces;

namespace Frontend.Core.Converting.Operations.CopyMod
{
    public class ModCopier : IModCopier
    {
        private readonly IDirectoryCopyHelper directoryCopyHelper;
        private readonly IDirectoryHelper directoryHelper;
        private readonly IFileProxy fileProxy;
        private readonly IFolderProxy folderProxy;
        private readonly IMessageBoxProxy messageBoxProxy;
        private readonly ISaveGameNameTranslator nameTranslator;
        private readonly IConverterOptions options;

        public ModCopier(
            IConverterOptions options,
            IFileProxy fileProxy,
            IFolderProxy folderProxy,
            IDirectoryHelper directoryHelper,
            IMessageBoxProxy messageBoxProxy,
            IDirectoryCopyHelper directoryCopyHelper,
            ISaveGameNameTranslator nameTranslator)
        {
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

            var targetGameModPathItem =
                options.CurrentConverter.RequiredItems.First(i => i.InternalTagName.Equals("targetGameModPath"));
            var desiredFileName =
                folderProxy.GetFileNameWithoutExtension(options.CurrentConverter.AbsoluteSourceSaveGame.SelectedValue) +
                options.CurrentConverter.TargetGame.SaveGameExtension;

            var translatedSaveGameName = nameTranslator.TranslateName(desiredFileName);

            // Copy the newly created output mod to the target game mod directory. 
            // The mod consists of two things: One file and one folder named after the save. Ex: The folder "France144_11_11" and the file "France144_11_11.mod".
            var converterWorkingDirectory = directoryHelper.GetConverterWorkingDirectory(options.CurrentConverter);
            var outputModFolderSourcePath = folderProxy.Combine(converterWorkingDirectory, "output",
                translatedSaveGameName);
            var outputModFileSourcePath = folderProxy.Combine(converterWorkingDirectory, "output",
                (translatedSaveGameName + ".mod"));

            var expectedAbsoluteOutputModFolderTargetPath = folderProxy.Combine(targetGameModPathItem.SelectedValue,
                translatedSaveGameName);
            var expectedAbsoluteOutputModFileTargetPath = expectedAbsoluteOutputModFolderTargetPath + ".mod";

            var modFileExists = fileProxy.Exists(expectedAbsoluteOutputModFileTargetPath);
            var modFolderExists = folderProxy.Exists(expectedAbsoluteOutputModFolderTargetPath);

            if (modFileExists || modFolderExists)
            {
                var confirmationMessage =
                    string.Format("One or more parts of the output mod exists in {0} already. Overwrite?",
                        targetGameModPathItem.SelectedValue);
                var result = messageBoxProxy.Show(confirmationMessage, "Confirmation Required", MessageBoxButton.YesNo);

                if (result == MessageBoxResult.No)
                {
                    operationResult.State = OperationResultState.Warning;
                    return operationResult;
                }
            }

            try
            {
                fileProxy.Copy(outputModFileSourcePath, expectedAbsoluteOutputModFileTargetPath, true);
                directoryCopyHelper.DirectoryCopy(outputModFolderSourcePath, expectedAbsoluteOutputModFolderTargetPath,
                    true, true);

                operationResult.LogEntries.Add(new LogEntry("Mod copied to: ", LogEntrySeverity.Info, LogEntrySource.UI,
                    expectedAbsoluteOutputModFolderTargetPath));
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