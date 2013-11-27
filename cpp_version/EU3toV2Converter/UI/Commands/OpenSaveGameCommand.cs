using System;
using Converter.UI.Settings;
using Microsoft.Win32;
using Converter.UI.Enums;

namespace Converter.UI.Commands
{
    /// <summary>
    /// Command used to select the save game that should be converted.
    /// </summary>
    public class OpenSaveGameCommand : CommandBase
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="OpenSaveGameCommand"/> class.
        /// </summary>
        /// <param name="options">The options.</param>
        public OpenSaveGameCommand(ConverterOptions options)
            : base (options)
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
            dialog.InitialDirectory = this.Options.SourceGame.SaveGamePath;
            Nullable<bool> result = dialog.ShowDialog();

            if (result == true)
            {
                this.Options.SourceSaveGame = dialog.FileName;
                this.Options.Logger.AddLogEntry(new LogEntry("Selected savegame " + this.Options.SourceSaveGame, LogEntrySeverity.Info, LogEntrySource.UI));
            }
        }
    }
}
