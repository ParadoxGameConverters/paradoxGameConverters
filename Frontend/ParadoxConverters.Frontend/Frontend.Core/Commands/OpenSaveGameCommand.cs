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
    /// Command used to select the save game that should be converted.
    /// </summary>
    public class OpenSaveGameCommand : CommandBase
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="OpenSaveGameCommand"/> class.
        /// </summary>
        /// <param name="eventAggregator"></param>
        /// <param name="options">The options.</param>
        public OpenSaveGameCommand(IEventAggregator eventAggregator, IConverterOptions options)
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
            OpenFileDialog dialog = new OpenFileDialog();

            // TODO: Read this from gameconfiguration instead.
            dialog.DefaultExt = this.Options.SourceGame.SaveGameExtension;
            dialog.Filter = this.Options.SourceGame.FriendlyName + " save games (*" + this.Options.SourceGame.SaveGameExtension + ") | *" + this.Options.SourceGame.SaveGameExtension;
            dialog.InitialDirectory = this.Options.SourceGame.AbsoluteSaveGamePath;
            Nullable<bool> result = dialog.ShowDialog();

            if (result == true)
            {
                this.Options.AbsoluteSourceSaveGamePath = dialog.FileName;
                this.EventAggregator.PublishOnUIThread(
                    new LogEntry("Selected savegame " + this.Options.AbsoluteSourceSaveGamePath, LogEntrySeverity.Info, LogEntrySource.UI, this.Options.AbsoluteSourceSaveGamePath));
            }
        }
    }
}
