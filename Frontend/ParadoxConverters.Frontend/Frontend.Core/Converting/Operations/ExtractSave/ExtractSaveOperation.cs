using System;
using System.IO;
using System.Threading.Tasks;
using Frontend.Core.Common.Proxies;
using Frontend.Core.Logging;
using Frontend.Core.Model.Interfaces;

namespace Frontend.Core.Converting.Operations.ExtractSave
{
    public class ExtractSaveOperation : IOperation
    {
        private readonly IEnvironmentProxy environmentProxy;
        private readonly IConverterOptions options;
        private readonly ICompressedSaveChecker saveChecker;
        private readonly IZipFileHelper zipFileHelper;

        public ExtractSaveOperation(
            IConverterOptions options,
            ICompressedSaveChecker saveChecker,
            IZipFileHelper zipFileHelper,
            IEnvironmentProxy environmentProxy)
        {
            this.options = options;
            this.saveChecker = saveChecker;
            this.zipFileHelper = zipFileHelper;
            this.environmentProxy = environmentProxy;
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
                this.zipFileHelper.ExtractFile(savePath, saveFileName, extractPath);
                this.options.CurrentConverter.AbsoluteSourceSaveGame.SelectedValue = Path.Combine(extractPath,
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
            var isCompressed =
                saveChecker.IsCompressedSave(options.CurrentConverter.AbsoluteSourceSaveGame.SelectedValue);

            return hasSaveSet && isCompressed;
        }
    }
}