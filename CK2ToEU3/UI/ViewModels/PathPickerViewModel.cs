using System;
using System.IO;
using System.Windows.Input;
using Converter.UI.Commands;
using Converter.UI.Framework;
using Converter.UI.Settings;
using Microsoft.Win32;
using Converter.UI.Enums;
using Converter.UI.Views;

namespace Converter.UI.ViewModels
{
    /// <summary>
    /// The PathPickerViewModel lets the user select paths to various stuff the converter needs to know where to find.
    /// </summary>
    public class PathPickerViewModel : BaseViewModel, IViewModel
    {
        private ICommand openSaveGameCommand;
        private ICommand openConverterCommand;
        private ICommand openFolderCommand;

        /// <summary>
        /// Initializes a new instance of the <see cref="PathPickerViewModel"/> class.
        /// </summary>
        /// <param name="options">The options.</param>
        /// <param name="view">The view.</param>
        public PathPickerViewModel(ConverterOptions options, IView view)
            : base(view, "File/Folder paths", options)
        {
            this.BuildConversionOptions();
        }

        #region [ Properties ]

        /// <summary>
        /// Gets the open converter command.
        /// </summary>
        /// <value>
        /// The open converter command.
        /// </value>
        public ICommand OpenConverterCommand
        {
            get
            {
                return this.openConverterCommand ?? (this.openConverterCommand = new OpenConverterCommand(this.Options));
            }
        }

        /// <summary>
        /// Gets the open save game command.
        /// </summary>
        /// <value>
        /// The open save game command.
        /// </value>
        public ICommand OpenSaveGameCommand
        {
            get
            {
                return this.openSaveGameCommand ?? (this.openSaveGameCommand = new OpenSaveGameCommand(this.Options));
            }
        }

        /// <summary>
        /// Gets the open folder command.
        /// </summary>
        /// <value>
        /// The open folder command.
        /// </value>
        public ICommand OpenFolderCommand
        {
            get
            {
                return this.openFolderCommand ?? (this.openFolderCommand = new OpenFolderCommand(this.Options));
            }
        }

        /// <summary>
        /// Gets or sets the source save game path.
        /// </summary>
        /// <value>
        /// The source save game path.
        /// </value>
        public string SourceSaveGamePath
        {
            get
            {
                return this.Options.SourceSaveGame;
            }

            set
            {
                if (this.Options.SourceSaveGame == value)
                {
                    return;
                }

                this.Options.SourceSaveGame = value;
                this.RaisePropertyChanged("SourceSaveGamePath");
            }
        }

        #endregion

        #region [ Methods ]

        /// <summary>
        /// Called when [tab selected].
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="System.Windows.Controls.SelectionChangedEventArgs"/> instance containing the event data.</param>
        protected override void OnTabSelected(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            // If the converter .exe happens to be in the same directory as the UI, there is no reason to ask the user to find it anyway. So let's just do it for them.
            var converterLocation = Path.Combine(Environment.CurrentDirectory, "CK2ToEU3.exe");
            if (File.Exists(converterLocation))
            {
                this.Options.Converter = converterLocation;
            }
        }

        /// <summary>
        /// Builds the conversion options.
        /// </summary>
        private void BuildConversionOptions()
        {
            this.VerifyInstallation(this.Options.SourceGame);
            this.VerifyInstallation(this.Options.TargetGame);
        }
        
        /// <summary>
        /// Attempts to figure out whether the game is installed.
        /// </summary>
        /// <param name="gameConfiguration">The game configuration</param>
        /// <returns>True if found, false if not</returns>
        private void VerifyInstallation(GameConfiguration gameConfiguration)
        {
            // Check registry for steam id
            if (!string.IsNullOrEmpty(gameConfiguration.SteamId))
            {
                string expectedRegistryPath = @"Software\Valve\Steam\Apps\" + gameConfiguration.SteamId;
                RegistryKey regKey = Registry.CurrentUser.OpenSubKey(expectedRegistryPath);

                if (regKey != null)
                {
                    string value = regKey.GetValue("Installed").ToString();
                    gameConfiguration.IsInstalled = value.Equals("1");
                    this.Options.Logger.AddLogEntry(new LogEntry("Found Steam installation of " + gameConfiguration.FriendlyName, LogEntrySeverity.Info, LogEntrySource.UI));
                }
                else
                {
                    // not a steam game - if the registry key correctly points us to the app directory, then it's installed
                    if (!string.IsNullOrEmpty(gameConfiguration.InstallationPath))
                        gameConfiguration.IsInstalled = true;
                    this.Options.Logger.AddLogEntry(new LogEntry("Found non-Steam installation of " + gameConfiguration.FriendlyName, LogEntrySeverity.Info, LogEntrySource.UI));
                }
            }

            // If the game is not really installed, clear the installation folder
            if (!gameConfiguration.IsInstalled)
                gameConfiguration.InstallationPath = String.Empty;
        }        

        #endregion
    }
}
