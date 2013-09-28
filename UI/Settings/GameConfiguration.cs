using Converter.UI.Framework;
using System.Collections.Generic;

namespace Converter.UI.Settings
{
    /// <summary>
    /// This class encapsulates all the game-specific information the converter needs to know.
    /// </summary>
    public class GameConfiguration : NotifiableBase
    {
        private string installationPath;
        private string saveGamePath;
        private string modPath;
        private IList<SupportedMod> supportedMods;
        private SupportedMod currentMod;

        /// <summary>
        /// Gets or sets the mod path.
        /// </summary>
        /// <value>
        /// The mod path.
        /// </value>
        public string ModPath
        {
            get
            {
                return this.modPath;
            }

            set
            {
                if (this.modPath == value)
                {
                    return;
                }

                this.modPath = value;
                this.RaisePropertyChanged("ModPath");
            }
        }

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

        /// <summary>
        /// Gets or sets the name of the configuration file mod directory tag.
        /// </summary>
        /// <value>
        /// The name of the configuration file mod directory tag.
        /// </value>
        public string ConfigurationFileModDirectoryTagName { get; set; }

        /// <summary>
        /// Gets or sets the name of the current mod tag.
        /// </summary>
        /// <value>
        /// The name of the current mod tag.
        /// </value>
        public string CurrentModTagName { get; set; }

        /// <summary>
        /// Gets a list of supported mods, as read from Configuration.xml
        /// </summary>
        /// <value>
        /// The supported mods.
        /// </value>
        public IList<SupportedMod> SupportedMods
        {
            get
            {
                return this.supportedMods ?? (this.supportedMods = new List<SupportedMod>());
            }
        }

        /// <summary>
        /// Gets a value indicating whether this game is configured use mods. This depends on the information available in Configuration.xml
        /// </summary>
        public bool IsConfiguredToUseMods
        {
            get
            {
                return this.SupportedMods.Count > 0 
                    && !string.IsNullOrEmpty(this.CurrentModTagName) 
                    && this.CurrentMod != null 
                    && !string.IsNullOrEmpty(this.ConfigurationFileModDirectoryTagName)
                    && !string.IsNullOrEmpty(this.ModPath);
            }
        }

        /// <summary>
        /// Gets or sets the selected mod.
        /// </summary>
        /// <value>
        /// The selected mod.
        /// </value>
        public SupportedMod CurrentMod
        {
            get
            {
                return this.currentMod;
            }

            set
            {
                if (this.currentMod == value)
                {
                    return;
                }

                this.currentMod = value;
                this.RaisePropertyChanged("CurrentMod");
            }
        }
    }
}
