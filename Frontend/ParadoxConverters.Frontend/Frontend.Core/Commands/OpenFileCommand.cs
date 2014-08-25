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

            if (!string.IsNullOrEmpty(requiredFile.ExpectedFileName))
            {
                dialog.Filter = requiredFile.ExpectedFileName + " | " + requiredFile.ExpectedFileName;
            }

            //if (this.Options != null && this.Options.CurrentConverter != null && this.Options.CurrentConverter.ConverterExeName != null)
            //{
            //    dialog.Filter = this.Options.CurrentConverter.ConverterExeName + " | " + this.Options.CurrentConverter.ConverterExeName; //"CK2ToEU3.exe | CK2ToEU3.exe";
            //}

            dialog.InitialDirectory = requiredFile.DefaultValue;
            Nullable<bool> result = dialog.ShowDialog();

            if (result == true)
            {
                //this.Options.CurrentConverter.AbsoluteConverterPath = dialog.FileName;
                requiredFile.SelectedValue = dialog.FileName;
                this.EventAggregator.PublishOnUIThread(
                    new LogEntry("Found " + requiredFile.FriendlyName + " at ", LogEntrySeverity.Info, LogEntrySource.UI, requiredFile.SelectedValue));
            }
        }
    }
}
