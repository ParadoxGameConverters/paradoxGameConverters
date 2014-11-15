using Caliburn.Micro;
using Frontend.Core.Logging;
using Frontend.Core.Model.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Win32;
using Frontend.Core.Model.Paths.Interfaces;
using System.IO;

namespace Frontend.Core.Commands
{
    public class OpenFileCommand : CommandBase
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="OpenFileCommand"/> class.
        /// </summary>
        /// <param name="eventAggregator"></param>
        /// <param name="options">The options.</param>
        public OpenFileCommand(IEventAggregator eventAggregator, IConverterOptions options)
            : base(eventAggregator, options)
        {
        }

        /// <summary>
        /// Called when [execute].
        /// </summary>
        /// <param name="parameter">The parameter.</param>
        protected override void OnExecute(object parameter)
        {
            var requiredFile = parameter as IRequiredFile;

            if (requiredFile == null)
            {
                return;
            }

            OpenFileDialog dialog = new OpenFileDialog();

            dialog.DefaultExt = requiredFile.Extension;
            
            //NOTE: The dialog doesn't handle InitialDirectories that contains file names. So, if a predefined filename is specified (and included in the DefaultValue property)
            // we need to strip it away for the purpose of setting the dialog.InitialDirectory property. 
            string pathMinusFileName;

            if (!string.IsNullOrEmpty(requiredFile.PredefinedFileName))
            {
                dialog.Filter = requiredFile.PredefinedFileName + " | " + requiredFile.PredefinedFileName;
                
                pathMinusFileName = Path.GetDirectoryName(requiredFile.DefaultValue);
            }
            else
            {
                pathMinusFileName = requiredFile.DefaultValue;
            }

            dialog.InitialDirectory = pathMinusFileName;
            Nullable<bool> result = dialog.ShowDialog();

            if (result == true)
            {
                requiredFile.SelectedValue = dialog.FileName;
                this.EventAggregator.PublishOnUIThread(
                    new LogEntry("Found " + requiredFile.FriendlyName + " at ", LogEntrySeverity.Info, LogEntrySource.UI, requiredFile.SelectedValue));
            }
        }
    }
}
