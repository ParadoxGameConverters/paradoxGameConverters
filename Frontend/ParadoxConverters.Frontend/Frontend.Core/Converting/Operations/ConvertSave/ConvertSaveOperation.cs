using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using Frontend.Core.Common.Proxies;
using Frontend.Core.Helpers;
using Frontend.Core.Logging;
using Frontend.Core.Model.Interfaces;

namespace Frontend.Core.Converting.Operations.ConvertSave
{
    public class ConvertSaveOperation : IOperation
    {
        private readonly IDirectoryHelper directoryHelper;
        private readonly IFileProxy fileProxy;
        private readonly IConverterOptions options;

        public ConvertSaveOperation(IConverterOptions options, IFileProxy fileProxy, IDirectoryHelper directoryHelper)
        {
            this.options = options;
            this.fileProxy = fileProxy;
            this.directoryHelper = directoryHelper;
        }

        public string Description
        {
            get { return "Converting save..."; }
        }

        public bool CanRun()
        {
            // Check various requirements
            var saveGame = options.CurrentConverter.RequiredItems.First(i => i.TagName.Equals("SaveGame"));

            if (saveGame == null || !fileProxy.Exists(saveGame.SelectedValue))
            {
                return false;
            }

            if (fileProxy.Exists(options.CurrentConverter.AbsoluteConverter.SelectedValue))
            {
                return true;
            }

            return false;
        }

        public OperationResult Process()
        {
            var result = new OperationResult();
            var task = Task.FromResult(result);

            // Reading process output syncronously. The async part is already handled by the command
            using (var process = new Process())
            {
                var argument = "\"" + options.CurrentConverter.AbsoluteSourceSaveGame.SelectedValue + "\"";

                process.StartInfo = new ProcessStartInfo
                {
                    FileName = options.CurrentConverter.AbsoluteConverter.SelectedValue,
                    Arguments = argument,
                    CreateNoWindow = true,
                    RedirectStandardError = true,
                    RedirectStandardInput = true,
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    WorkingDirectory = directoryHelper.GetConverterWorkingDirectory(options.CurrentConverter)
                };

                //result.LogEntries.Add(new LogEntry("Converting - this may take a few minutes...", LogEntrySeverity.Info, LogEntrySource.UI, null));
                //Thread.Sleep(100); // Sleeping may let the UI actually display the above message before starting the conversion process

                var stopwatch = new Stopwatch();
                stopwatch.Start();

                process.Start();

                // A non-working attempt to actually log the converter output while converting. 
                // As of this writing, I don't know if this fails due to the converter or something on the frontend side - it could be both.
                while (!process.StandardOutput.EndOfStream)
                {
                    result.LogEntries.Add(new LogEntry(process.StandardOutput.ReadLine(), LogEntrySeverity.Info,
                        LogEntrySource.Converter, null));
                }

                process.WaitForExit();

                stopwatch.Stop();

                if (process.ExitCode == 0)
                {
                    // TODO:REMOVE
                    options.WasConversionSuccessful = true;
                    //result.LogEntries.Add(new LogEntry("Conversion complete after " + this.BuildTimeSpanString(stopwatch.Elapsed), LogEntrySeverity.Info, LogEntrySource.UI, null));
                }
                else
                {
                    // TODO:REMOVE
                    options.WasConversionSuccessful = false;
                    result.State = OperationResultState.Error;
                    //result.LogEntries.Add(new LogEntry("Conversion failed after" + this.BuildTimeSpanString(stopwatch.Elapsed), LogEntrySeverity.Error, LogEntrySource.UI, null));
                }
            }

            //Thread.Sleep(3000);

            return task.Result;
        }
    }
}