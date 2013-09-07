using Converter.UI.Framework;

namespace Converter.UI.Settings
{
    /// <summary>
    /// This class encapsulates all the game-specific information the converter needs to know.
    /// </summary>
    public class GameConfiguration : NotifiableBase
    {
        private string installationPath;
        private string saveGamePath;

        /// <summary>
        /// Gets or sets the installation path.
        /// </summary>
        /// <value>
        /// The installation path.
        /// </value>
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

        /// <summary>
        /// Gets or sets the save game path.
        /// </summary>
        /// <value>
        /// The save game path.
        /// </value>
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

        /// <summary>
        /// Gets or sets the name.
        /// </summary>
        /// <value>
        /// The name.
        /// </value>
        public string Name { get; set; }

        /// <summary>
        /// Gets or sets the name of the friendly.
        /// </summary>
        /// <value>
        /// The name of the friendly.
        /// </value>
        public string FriendlyName { get; set; }

        /// <summary>
        /// Gets or sets the save game extension.
        /// </summary>
        /// <value>
        /// The save game extension.
        /// </value>
        public string SaveGameExtension { get; set; }

        /// <summary>
        /// Gets or sets the steam unique identifier.
        /// </summary>
        /// <value>
        /// The steam unique identifier.
        /// </value>
        public string SteamId { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether [is installed].
        /// </summary>
        /// <value>
        ///   <c>true</c> if [is installed]; otherwise, <c>false</c>.
        /// </value>
        public bool IsInstalled { get; set; }

        /// <summary>
        /// Gets or sets the version.
        /// </summary>
        /// <value>
        /// The version.
        /// </value>
        public string Version { get; set; }

        /// <summary>
        /// Gets or sets the name of the configuration file directory tag.
        /// </summary>
        /// <value>
        /// The name of the configuration file directory tag.
        /// </value>
        public string ConfigurationFileDirectoryTagName { get; set; }
    }
}
