using Frontend.Core.Logging;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Windows.Forms;
using Caliburn.Micro;
using Frontend.Core.Model.Interfaces;

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
            IGameConfiguration game = (IGameConfiguration)parameter;

            if (game == null)
            {
                return;
            }

            var dialog = new FolderBrowserDialog();

            if (Directory.Exists(game.AbsoluteInstallationPath))
            {
                dialog.SelectedPath = game.AbsoluteInstallationPath;
            }

            dialog.ShowNewFolderButton = false;

            DialogResult result = dialog.ShowDialog();

            if (result == DialogResult.OK)
            {
                game.AbsoluteInstallationPath = dialog.SelectedPath;
                this.EventAggregator.PublishOnUIThread(new LogEntry("Updated installation folder for " + game.FriendlyName + "; new directory is " + game.AbsoluteInstallationPath, LogEntrySeverity.Info, LogEntrySource.UI, game.AbsoluteInstallationPath));
            }
        }
    }
}
