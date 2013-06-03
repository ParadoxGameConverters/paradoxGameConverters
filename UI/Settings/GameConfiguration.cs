using Converter.UI.Framework;

namespace Converter.UI.Settings
{
    public class GameConfiguration : NotifiableBase
    {
        private string installationPath;
        private string saveGamePath;

        public string InstallationPath
        {
            get
            {
                return this.installationPath;
            }

            set
            {
                if (this.installationPath == value)
                {
                    return;
                }

                this.installationPath = value;
                this.RaisePropertyChanged("InstallationPath");
            }
        }

        public string SaveGamePath
        {
            get
            {
                return this.saveGamePath;
            }

            set
            {
                if (this.saveGamePath == value)
                {
                    return;
                }

                this.saveGamePath = value;
                this.RaisePropertyChanged("SaveGamePath");
            }
        }

        public string Name { get; set; }
        public string FriendlyName { get; set; }
        public string SteamId { get; set; }
        public bool IsInstalled { get; set; }
        public string Version { get; set; }
        public string ConfigurationFileDirectoryTagName { get; set; }
    }
}
