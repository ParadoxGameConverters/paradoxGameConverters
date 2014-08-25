using Caliburn.Micro;
using Frontend.Core.Logging;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.Model.Paths.Interfaces;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace Frontend.Core.Commands
{
    /// <summary>
    /// Command used to select a particular folder
    /// </summary>
    public class OpenFolderCommand : CommandBase
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="OpenFolderCommand"/> class.
        /// </summary>
        /// <param name="eventAggregator"></param>
        /// <param name="options">The options.</param>
        public OpenFolderCommand(IEventAggregator eventAggregator, IConverterOptions options)
            : base(eventAggregator, options)
        {
        }

        /// <summary>
        /// Called when [can execute].
        /// </summary>
        /// <param name="parameter">The parameter.</param>
        /// <returns></returns>
        protected override bool OnCanExecute(object parameter)
        {
            return true;
        }

        /// <summary>
        /// Called when [execute].
        /// </summary>
        /// <param name="parameter">The parameter.</param>
        protected override void OnExecute(object parameter)
        {
            var requiredItem = parameter as IRequiredItemBase;
            
            if (requiredItem == null)
            {
                return;
            }

            var dialog = new FolderBrowserDialog();

            if (Directory.Exists(requiredItem.DefaultValue))
            {
                dialog.SelectedPath = requiredItem.DefaultValue;
            } 
            
            dialog.ShowNewFolderButton = false;

            DialogResult result = dialog.ShowDialog();

            if (result == DialogResult.OK)
            {
                requiredItem.SelectedValue = dialog.SelectedPath;
                this.EventAggregator.PublishOnUIThread(new LogEntry("Successfully set " + requiredItem.FriendlyName + " to ", LogEntrySeverity.Info, LogEntrySource.UI, requiredItem.SelectedValue));
            }
        }
    }
}
