using System.Diagnostics;
using System.IO;
using Caliburn.Micro;
using Frontend.Core.Logging;

namespace Frontend.Core.Commands
{
    public class OpenUriCommand : CommandBase
    {
        public OpenUriCommand(IEventAggregator eventAggregator)
            : base(eventAggregator)
        {
        }

        protected override void OnExecute(object parameter)
        {
            var path = (string) parameter;

            if (string.IsNullOrEmpty(path))
            {
                return;
            }

            var isFile = File.Exists(path);
            var isDirectory = Directory.Exists(path);

            try
            {
                if (isDirectory)
                {
                    Process.Start(path);
                }
                else if (isFile && !isDirectory)
                {
                    var args = string.Format("/Select, {0}", path);

                    var pfi = new ProcessStartInfo("Explorer.exe", args);
                    Process.Start(pfi);
                }
                else
                {
                    var folderWhereFileWasExpectedToExist = Path.GetDirectoryName(path);
                    if (!string.IsNullOrEmpty(folderWhereFileWasExpectedToExist))
                    {
                        Process.Start(folderWhereFileWasExpectedToExist);
                    }
                }
            }
            catch
            {
                EventAggregator.PublishOnUIThread(
                    new LogEntry(
                        "Opening the directory (and/or selecting the file failed miserably. Please try manually instead.",
                        LogEntrySeverity.Error, LogEntrySource.UI));
            }
        }
    }
}