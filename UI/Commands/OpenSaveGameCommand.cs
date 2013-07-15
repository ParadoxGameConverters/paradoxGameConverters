using System;
using Converter.UI.Settings;
using Microsoft.Win32;
using Converter.UI.Enums;

namespace Converter.UI.Commands
{
    public class OpenSaveGameCommand : CommandBase
    {
        public OpenSaveGameCommand(ConverterOptions options)
            : base (options)
        {
        }

        protected override bool OnCanExecute(object parameter)
        {
            return true;
        }

        protected override void OnExecute(object parameter)
        {
            OpenFileDialog dialog = new OpenFileDialog();

            dialog.DefaultExt = ".ck2";
            dialog.Filter = "Crusader Kings 2 save games (.ck2) | *.ck2";
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
