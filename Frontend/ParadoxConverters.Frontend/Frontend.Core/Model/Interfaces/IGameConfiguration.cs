using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;

namespace Frontend.Core.Model.Interfaces
{
    public interface IGameConfiguration
    {
        ///// <summary>
        ///// Gets or sets the mod path.
        ///// </summary>
        ///// <value>
        ///// The mod path.
        ///// </value>
        //string AbsoluteModPath { get; set; }

        /// <summary>
        /// Gets or sets the installation path.
        /// </summary>
        /// <value>
        /// The installation path.
        /// </value>
        string AbsoluteInstallationPath { get; set; }

        ///// <summary>
        ///// Gets or sets the name of the file where we get the icon
        ///// </summary>
        //string AbsoluteIconFilePath { get; set; }

        ///// <summary>
        ///// Gets or sets the game Icon
        ///// </summary>
        //ImageSource Icon { get; set; }

        /// <summary>
        /// Gets or sets the save game path.
        /// </summary>
        /// <value>
        /// The save game path.
        /// </value>
        string AbsoluteSaveGamePath { get; set; }

        /// <summary>
        /// Gets or sets the name.
        /// </summary>
        /// <value>
        /// The name.
        /// </value>
        string Name { get; set; }

        /// <summary>
        /// Gets or sets the name of the friendly.
        /// </summary>
        /// <value>
        /// The name of the friendly.
        /// </value>
        string FriendlyName { get; set; }

        /// <summary>
        /// Gets or sets the save game extension.
        /// </summary>
        /// <value>
        /// The save game extension.
        /// </value>
        string SaveGameExtension { get; set; }

        /// <summary>
        /// Gets or sets the steam unique identifier.
        /// </summary>
        /// <value>
        /// The steam unique identifier.
        /// </value>
        string SteamId { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether [is installed].
        /// </summary>
        /// <value>
        ///   <c>true</c> if [is installed]; otherwise, <c>false</c>.
        /// </value>
        bool IsInstalled { get; set; }

        /// <summary>
        /// Gets or sets the version.
        /// </summary>
        /// <value>
        /// The version.
        /// </value>
        string Version { get; set; }

        /// <summary>
        /// Gets or sets the name of the configuration file directory tag.
        /// </summary>
        /// <value>
        /// The name of the configuration file directory tag.
        /// </value>
        string ConfigurationFileDirectoryTagName { get; set; }

        ///// <summary>
        ///// Gets or sets the name of the configuration file mod directory tag.
        ///// </summary>
        ///// <value>
        ///// The name of the configuration file mod directory tag.
        ///// </value>
        //string ConfigurationFileModDirectoryTagName { get; set; }

        ///// <summary>
        ///// Gets or sets the name of the current mod tag.
        ///// </summary>
        ///// <value>
        ///// The name of the current mod tag.
        ///// </value>
        //string CurrentModTagName { get; set; }

        ///// <summary>
        ///// Gets a list of supported mods, as read from Configuration.xml
        ///// </summary>
        ///// <value>
        ///// The supported mods.
        ///// </value>
        //IList<SupportedMod> SupportedMods { get; }

        ///// <summary>
        ///// Gets a value indicating whether this game is configured use mods. This depends on the information available in Configuration.xml
        ///// </summary>
        //bool IsConfiguredToUseMods { get; }

        ///// <summary>
        ///// Gets or sets the selected mod.
        ///// </summary>
        ///// <value>
        ///// The selected mod.
        ///// </value>
        //SupportedMod CurrentMod { get; set; }
    }
}
