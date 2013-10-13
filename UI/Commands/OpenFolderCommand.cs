using System.IO;
using System.Windows.Forms;
using Converter.UI.Settings;
using Converter.UI.Enums;

namespace Converter.UI.Commands
{
    /// <summary>
    /// Command used to select a particular folder
    /// </summary>
    public class OpenFolderCommand : CommandBase
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="OpenFolderCommand"/> class.
        /// </summary>
        /// <param name="options">The options.</param>
        public OpenFolderCommand(ConverterOptions options)
            : base(options)
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
            GameConfiguration game = (GameConfiguration)parameter;

            if (game == null)
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
