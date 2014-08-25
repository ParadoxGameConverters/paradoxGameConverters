using Caliburn.Micro;
using Frontend.Core.Events.EventArgs;
using Frontend.Core.Logging;
using Frontend.Core.Model.Interfaces;
using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.IO;
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
            //OpenFileDialog dialog = new OpenFileDialog();

            //dialog.DefaultExt = this.Options.CurrentConverter.SourceGame.SaveGameExtension;
            //dialog.Filter = this.Options.CurrentConverter.SourceGame.FriendlyName + " save games (*" + this.Options.CurrentConverter.SourceGame.SaveGameExtension + ") | *" + this.Options.CurrentConverter.SourceGame.SaveGameExtension;

            //// Just default to the current working directory if the default save game path doesn't exist. 
            //// This can typically occur if the user just installed the game, and haven't made any saves yet.
            //if (Directory.Exists(this.Options.CurrentConverter.AbsoluteSourceSaveGamePath))
            //{
            //    dialog.InitialDirectory = this.Options.CurrentConverter.AbsoluteSourceSaveGamePath;
            //}
            //else
            //{
            //    dialog.InitialDirectory = Environment.CurrentDirectory;
            //}

            //Nullable<bool> result = dialog.ShowDialog();

            //if (result == true)
            //{
            //    this.Options.CurrentConverter.AbsoluteSourceSaveGamePath = dialog.FileName;
            //    this.EventAggregator.PublishOnUIThread(
            //        new LogEntry("Selected savegame", LogEntrySeverity.Info, LogEntrySource.UI, this.Options.CurrentConverter.AbsoluteSourceSaveGamePath));
            //    this.EventAggregator.BeginPublishOnUIThread(new RefreshButtonStatesArgs());
            //}
        }
    }
}
