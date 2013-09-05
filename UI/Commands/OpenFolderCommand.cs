using System.IO;
using System.Windows.Forms;
using Converter.UI.Settings;
using Converter.UI.Enums;

namespace Converter.UI.Commands
{
    public class OpenFolderCommand : CommandBase
    {
        public OpenFolderCommand(ConverterOptions options)
            : base(options)
        {
        }

        protected override bool OnCanExecute(object parameter)
        {
            return true;
        }

        protected override void OnExecute(object parameter)
        {
            GameConfiguration game = (GameConfiguration)parameter;

            if (game == null || string.IsNullOrEmpty(game.InstallationPath))
            {
                return;
            }

            var dialog = new FolderBrowserDialog();

            if (Directory.Exists(game.InstallationPath))
            {
                dialog.SelectedPath = game.InstallationPath;
            }

            dialog.ShowNewFolderButton = false;
            
            DialogResult result = dialog.ShowDialog();

            if(result == DialogResult.OK)
            {
                game.InstallationPath = dialog.SelectedPath;
                this.Options.Logger.AddLogEntry(new LogEntry("Updated installation folder for " + game.FriendlyName + "; new directory is " + game.InstallationPath, LogEntrySeverity.Info, LogEntrySource.UI));
            }
        }
    }
}
