using Caliburn.Micro;
using Frontend.Core.Logging;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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
            var path = (string)parameter;

            if (String.IsNullOrEmpty(path))
            {
                return;
            }

            bool isFile = File.Exists(path);
            bool isDirectory = Directory.Exists(path);

            try
            {
                if (isDirectory)
                {
                    Process.Start(path);
                }
                else if (isFile && !isDirectory)
                {
                    string args = string.Format("/Select, {0}", path);

                    ProcessStartInfo pfi = new ProcessStartInfo("Explorer.exe", args);
                    System.Diagnostics.Process.Start(pfi);
                }
            }
            catch
            {
                this.EventAggregator.PublishOnUIThread(new LogEntry("Opening the directory (and/or selecting the file failed miserably. Please try manually instead.", LogEntrySeverity.Error, LogEntrySource.UI));
            }
        }
    }
}
