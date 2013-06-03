using System;
using System.IO;
using System.Windows.Input;
using Converter.UI.Commands;
using Converter.UI.Framework;
using Converter.UI.Settings;
using Microsoft.Win32;

namespace Converter.UI.ViewModels
{
    public class SaveGamePickerViewModel : BaseViewModel, IViewModel
    {
        private ICommand openSaveGameCommand;
        private ICommand openFolderCommand;

        public SaveGamePickerViewModel(ConverterOptions options, IView view)
            : base(view, "File/Folder paths", options)
        {
            this.BuildConversionOptions();
        }

        #region [ Properties ]

        public ICommand OpenSaveGameCommand
        {
            get
            {
                return this.openSaveGameCommand ?? (this.openSaveGameCommand = new OpenSaveGameCommand(this.Options));
            }
        }

        public ICommand OpenFolderCommand
        {
            get
            {
                return this.openFolderCommand ?? (this.openFolderCommand = new OpenFolderCommand(this.Options));
            }
        }
        
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

        private void BuildConversionOptions()
        {
            //TODO: Finding installation paths makes little sense if the game isn't installed. Should probably be fixed
            this.VerifyInstallation(this.Options.SourceGame);
            this.VerifyInstallation(this.Options.TargetGame);
            this.FindGameInstallationPath(this.Options.SourceGame);
            this.FindGameInstallationPath(this.Options.TargetGame);
        }

        private void FindGameInstallationPath(GameConfiguration gameConfiguration)
        {
            if (!gameConfiguration.IsInstalled)
            {
                return;
            }

            // If installed via Steam, find game installation path.
            // Easiest way I've found so far is to look at the uninstall settings.
            if (!string.IsNullOrEmpty(gameConfiguration.SteamId))
            {
                RegistryKey regKey = Registry.LocalMachine.OpenSubKey(@"SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\Steam App " + gameConfiguration.SteamId);

                if (regKey != null)
                {
                    string steamInstallationPath = regKey.GetValue("InstallLocation").ToString();

                    if (!String.IsNullOrEmpty(steamInstallationPath))
                    {
                        if (Directory.Exists(steamInstallationPath))
                        {
                            gameConfiguration.InstallationPath = steamInstallationPath;
                            ////this.LogItems.Add(new LogEntry("Located Steam game files: " + steamInstallationPath));
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Attempts to figure out whether the game is installed.
        /// 
        /// Currently, only steam installs are supported.
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
                    //this.LogItems.Add(new LogEntry("Found Steam installation of " + gameConfiguration.FriendlyName));
                }
            }
        }        

        #endregion
    }
}
