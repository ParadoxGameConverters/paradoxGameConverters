using System;
using System.IO;
using Frontend.Core.Common.Proxies;
using Frontend.Core.Logging;
using Frontend.Core.Model.Interfaces;

namespace Frontend.Core.Converting.Operations.ExtractSave
{
    public class ExtractSaveOperation : IOperation
    {
        private readonly IEnvironmentProxy environmentProxy;
        private readonly IFileProxy _fileProxy;
        private readonly IFolderProxy _folderProxy;
        private readonly IConverterOptions options;
        private readonly ICompressedSaveChecker saveChecker;
        private readonly IZipFileHelper zipFileHelper;

        public ExtractSaveOperation(
            IConverterOptions options,
            ICompressedSaveChecker saveChecker,
            IZipFileHelper zipFileHelper,
            IEnvironmentProxy environmentProxy,
            IFileProxy fileProxy,
            IFolderProxy folderProxy)
        {
            this.options = options;
            this.saveChecker = saveChecker;
            this.zipFileHelper = zipFileHelper;
            this.environmentProxy = environmentProxy;
            _fileProxy = fileProxy;
            _folderProxy = folderProxy;
        }

        public string Description
        {
            get { return "Extracting save..."; }
        }

        public OperationResult Process()
        {
            var savePath = options.CurrentConverter.AbsoluteSourceSaveGame.SelectedValue;
            var extractPath = environmentProxy.GetFrontendWorkingDirectory();
            var result = new OperationResult();
            var saveFileName = Path.GetFileName(savePath);

            try
            {
                zipFileHelper.ExtractFile(savePath, saveFileName, extractPath);
                options.CurrentConverter.AbsoluteSourceSaveGame.SelectedValue = Path.Combine(extractPath,
                    saveFileName);
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
            var hasSaveSet = !string.IsNullOrEmpty(options.CurrentConverter.AbsoluteSourceSaveGame.SelectedValue);
            var isFolderAndNotFile =
                this.IsFolderAndNotFile(options.CurrentConverter.AbsoluteSourceSaveGame.SelectedValue);

            if (isFolderAndNotFile)
            {
                return false;
            }
            
            var isCompressed =
                saveChecker.IsCompressedSave(options.CurrentConverter.AbsoluteSourceSaveGame.SelectedValue);

            return hasSaveSet && isCompressed;
        }

        private bool IsFolderAndNotFile(string path)
        {
            var isFile = this._fileProxy.Exists(path);
            var isFolder = this._folderProxy.Exists(path);

            return !isFile && isFolder;
        }
    }
}