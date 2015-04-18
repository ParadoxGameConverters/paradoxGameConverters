using Frontend.Core.Common.Proxies;
using Frontend.Core.Helpers;
using Frontend.Core.Logging;
using Frontend.Core.Model;
using Frontend.Core.Model.Interfaces;
using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;

namespace Frontend.Core.Converting.Operations.ExtractSave
{
    public class ExtractSaveOperation : IOperation
    {
        private IConverterOptions options;
        private ICompressedSaveChecker saveChecker;
        private IDirectoryHelper directoryHelper;
        private IMessageBoxProxy messageBoxProxy;
        private IFileProxy fileProxy;

        public ExtractSaveOperation(
            IConverterOptions options,
            ICompressedSaveChecker saveChecker,
            IDirectoryHelper directoryHelper,
            IMessageBoxProxy messageBoxProxy,
            IFileProxy fileProxy)
        {
            this.options = options;
            this.saveChecker = saveChecker;
            this.directoryHelper = directoryHelper;
            this.messageBoxProxy = messageBoxProxy;
            this.fileProxy = fileProxy;
        }

        public string Description
        {
            get
            {
                return "Extracting save...";
            }
        }

        public async Task<OperationResult> Process()
        {
            var savePath = this.options.CurrentConverter.AbsoluteSourceSaveGame.SelectedValue;
            var extractPath = this.directoryHelper.GetFrontendWorkingDirectory();
            var saveFilePathAfterExtraction = Path.Combine(extractPath, Path.GetFileName(this.options.CurrentConverter.AbsoluteSourceSaveGame.SelectedValue));
            bool extractedSaveExists = this.fileProxy.Exists(saveFilePathAfterExtraction);

            var result = new OperationResult();

            if (extractedSaveExists)
            {
                var messageBoxText = String.Format("Save game {0} exists already. Delete before extracting?", saveFilePathAfterExtraction);
                var messageBoxResult = this.messageBoxProxy.Show(messageBoxText, "Save exists already", System.Windows.MessageBoxButton.YesNo);

                if (messageBoxResult == MessageBoxResult.Yes)
                {
                    this.fileProxy.DeleteFile(saveFilePathAfterExtraction);
                }
            }

            try
            {
                ZipFile.ExtractToDirectory(savePath, extractPath);
                result.State = OperationResultState.Success;
            }
            catch (Exception e)
            {
                result.State = OperationResultState.Error;
                result.LogEntries.Add(new LogEntry(e.Message, LogEntrySeverity.Error, LogEntrySource.UI));
            }

            return result;
        }

        public bool CanRun()
        {
            var hasSaveSet = !string.IsNullOrEmpty(this.options.CurrentConverter.AbsoluteSourceSaveGame.SelectedValue);
            //var isCompressedSave = this.saveChecker.CheckIfCompressedSave(this.options.CurrentConverter.AbsoluteSourceSaveGame.SelectedValue);

            return hasSaveSet; // && isCompressedSave;
        }
    }
}
