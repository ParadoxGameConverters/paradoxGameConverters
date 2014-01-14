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

        ///// <summary>
        ///// Gets or sets the name of the configuration file mod directory tag.
        ///// </summary>
        ///// <value>
        ///// The name of the configuration file mod directory tag.
        ///// </value>
        //public string ConfigurationFileModDirectoryTagName { get; set; }

        /// <summary>
        /// 
        /// </summary>
        public string AbsoluteInstallationPath { get; set; }
        
        /// <summary>
        /// 
        /// </summary>
        public string AbsoluteSaveGamePath { get; set; }
    }
}
