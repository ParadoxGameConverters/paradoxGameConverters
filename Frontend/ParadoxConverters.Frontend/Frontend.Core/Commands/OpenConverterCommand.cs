using Caliburn.Micro;
using Frontend.Core.Logging;
using Frontend.Core.Model.Interfaces;
using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Commands
{
    /// <summary>
    /// 
    /// </summary>
    public class OpenConverterCommand : CommandBase
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="OpenConverterCommand"/> class.
        /// </summary>
        /// <param name="eventAggregator"></param>
        /// <param name="options">The options.</param>
        public OpenConverterCommand(IEventAggregator eventAggregator, IConverterOptions options)
            : base(eventAggregator, options)
        {
        }
        
        /// <summary>
        /// Called when [execute].
        /// </summary>
        /// <param name="parameter">The parameter.</param>
        protected override void OnExecute(object parameter)
        {
            OpenFileDialog dialog = new OpenFileDialog();

            dialog.DefaultExt = ".exe";

            if (this.Options != null && this.Options.CurrentConverter != null && this.Options.CurrentConverter.ConverterExeName != null)
            {
                dialog.Filter = this.Options.CurrentConverter.ConverterExeName + " | " + this.Options.CurrentConverter.ConverterExeName; //"CK2ToEU3.exe | CK2ToEU3.exe";
            }
            
            dialog.InitialDirectory = this.Options.CurrentConverter.AbsoluteConverterPath;
            Nullable<bool> result = dialog.ShowDialog();

            if (result == true)
            {
                this.Options.CurrentConverter.AbsoluteConverterPath = dialog.FileName;
                this.EventAggregator.PublishOnUIThread(
                    new LogEntry("Converter .exe found at " + this.Options.CurrentConverter.AbsoluteConverterPath, LogEntrySeverity.Info, LogEntrySource.UI, this.Options.CurrentConverter.AbsoluteConverterPath));
            }
        }
    }
}
