using Caliburn.Micro;
using Frontend.Core.Model.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Model
{
    public class GameConfiguration : PropertyChangedBase, IGameConfiguration
    {
        /// <summary>
        /// Backing field for the <see cref="SupportedMods"/> collection.
        /// </summary>
        private IList<IMod> supportedMods;

        /// <summary>
        /// Backing store for the <see cref="AbsoluteModPath"/> property.
        /// </summary>
        private string absoluteModPath;

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
        /// 
        /// </summary>
        public string AbsoluteInstallationPath { get; set; }
        
        /// <summary>
        /// 
        /// </summary>
        public string AbsoluteSaveGamePath { get; set; }

        /// <summary>
        /// The current mod tag name (IE: What's the tag used to identify this in configuration.txt)
        /// </summary>
        public string CurrentModTagName { get; set; }

        public IList<IMod> SupportedMods
        {
            get 
            {
                return this.supportedMods ?? (this.supportedMods = new List<IMod>());
            }
        }

        /// <summary>
        /// A value indicating whether this game is configured to use mods
        /// </summary>
        public bool IsConfiguredToUseMods
        {
            get
            {
                return this.SupportedMods.Count > 0
                    && !string.IsNullOrEmpty(this.CurrentModTagName)
                    && this.CurrentMod != null
                    && !string.IsNullOrEmpty(this.ConfigurationFileModDirectoryTagName)
                    && !string.IsNullOrEmpty(this.AbsoluteModPath);
            }
        }

        /// <summary>
        /// Gets or sets the mod path.
        /// </summary>
        /// <value>
        /// The mod path.
        /// </value>
        public string AbsoluteModPath
        {
            get
            {
                return this.absoluteModPath;
            }

            set
            {
                if (this.absoluteModPath == value)
                {
                    return;
                }

                this.absoluteModPath = value;
                this.NotifyOfPropertyChange(() => this.AbsoluteModPath);
            }
        }

        /// <summary>
        /// The current mod
        /// </summary>
        public IMod CurrentMod { get; set; }
    }
}
