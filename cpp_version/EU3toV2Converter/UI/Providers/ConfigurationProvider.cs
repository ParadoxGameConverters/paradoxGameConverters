using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Windows.Forms;
using System.Xml;
using System.Xml.Linq;
using Converter.UI.Enums;
using Converter.UI.Framework;
using Converter.UI.Helpers;
using Converter.UI.Settings;
using Microsoft.Win32;

namespace Converter.UI.Providers
{
    /// <summary>
    /// This class is responsible for turning the data in configuration.xml into a set of properties on the ConverterOPtions object
    /// </summary>
    public class ConfigurationProvider
    {
        #region [ Fields ]

        private IList<GameConfiguration> sourceGames;
        private IList<GameConfiguration> targetGames;
        private IList<PreferenceCategory> preferenceCategories;
        private ConverterOptions options;

        #endregion

        #region [ Constructor ]

        /// <summary>
        /// Initializes a new instance of the <see cref="ConfigurationProvider"/> class.
        /// </summary>
        /// <param name="options">The options.</param>
        public ConfigurationProvider(ConverterOptions options)
        {
            this.options = options;
        }

        #endregion

        #region [ Properties ]

        #region [ Public Properties ]

        /// <summary>
        /// Gets the list of source games.
        /// </summary>
        /// <value>
        /// The source games.
        /// </value>
        public IList<GameConfiguration> SourceGames
        {
            get
            {
                return this.sourceGames ?? (this.sourceGames = this.ReadGameConfig("sourceGameConfigurations"));
            }
        }

        /// <summary>
        /// Gets the list of target games.
        /// </summary>
        /// <value>
        /// The target games.
        /// </value>
        public IList<GameConfiguration> TargetGames
        {
            get
            {
                return this.targetGames ?? (this.targetGames = this.ReadGameConfig("targetGameConfigurations"));
            }
        }

        /// <summary>
        /// Gets the list of preference categories.
        /// </summary>
        /// <value>
        /// The preference categories.
        /// </value>
        public IList<PreferenceCategory> PreferenceCategories
        {
            get
            {
                return this.preferenceCategories ?? (this.preferenceCategories = this.ReadCategoryConfig());
            }
        }

        #endregion
        
        #endregion

        #region [ Methods ]

        #region [ Private Methods ]

        #region [ Preference Retrieval ]

        /// <summary>
        /// Reads the category configuration.
        /// </summary>
        /// <returns></returns>
        private IList<PreferenceCategory> ReadCategoryConfig()
        {
            var categories = new List<PreferenceCategory>();
            var configFileName = "Configuration.xml";

            var config = XmlHelper.ReadXmlFile(configFileName);

            if (config == null)
            {
                return null;
            }

            try
            {
                // Read the list of categories from the xml
                var foundCategories = config.Descendants("category");
                
                // For each category, store the values in a new PreferenceCategory object.
                foreach (var foundCategory in foundCategories)
                {
                    var category = new PreferenceCategory();
                    category.FriendlyName = XElementHelper.ReadStringValue(foundCategory, "friendlyName");
                    categories.Add(category);

                    // Read the list of Preference tags
                    var foundPreferences = XElementHelper.ReadEnumerable(foundCategory, "preference");

                    // For each preference Tag, store the values into a new Preference object
                    foreach (var foundPreference in foundPreferences)
                    {
                        IPreference preference = new Preference();

                        preference.Name = XElementHelper.ReadStringValue(foundPreference, "name");
                        preference.FriendlyName = XElementHelper.ReadStringValue(foundPreference, "friendlyName");
                        preference.Description = XElementHelper.ReadStringValue(foundPreference, "description", false);
                        preference.MinValue = XElementHelper.ReadDoubleValue(foundPreference, "minValue", false);
                        preference.MaxValue = XElementHelper.ReadDoubleValue(foundPreference, "maxValue", false);
                        preference.Value = XElementHelper.ReadStringValue(foundPreference, "value", false);
                        preference.HasDirectlyEditableValue = XElementHelper.ReadBoolValue(foundPreference, "hasDirectlyEditableValue", false);

                        if (preference.IsDate)
                        {
                            var format = XElementHelper.ReadStringValue(foundPreference, "dateFormat");
                            preference.MinDateValue = XElementHelper.ReadDateValue(foundPreference, "minDateValue", format, false);
                            preference.MaxDateValue = XElementHelper.ReadDateValue(foundPreference, "maxDateValue", format, false);
                        }
                        
                        category.Preferences.Add(preference);

                        // Read the list of entryOption tags
                        var foundEntries = XElementHelper.ReadEnumerable(foundPreference, "entryOption", false);

                        // For each tag, read the values into a new PreferenceEntry object
                        foreach (var entry in foundEntries)
                        {
                            preference.Entries.Add(this.BuildPreferenceEntry(preference, entry));
                        }
                    }
                }
            }
            ////catch (MissingRequiredXMLPropertyException ex)
            ////{
            ////}
            ////catch (MalformedXMLException ex)
            ////{
            ////}
            ////catch (UnParsableDataValueException ex)
            ////{
            ////}
            catch (XMLParseExceptionBase ex)
            {
                this.ShowMalformedOrMissingXMLElementError(configFileName, ex);
            }
            catch (Exception ex)
            {
                this.options.Logger.AddLogEntry(new LogEntry(ex.Message + " - " + ex.InnerException, LogEntrySeverity.Error, LogEntrySource.UI));
            }

            return categories;
        }

        #endregion

        #region [ Preference Entry Construction ]

        /// <summary>
        /// Builds a preference entry for the provided parent based on the provided XElement node
        /// </summary>
        /// <param name="parent">The parent.</param>
        /// <param name="foundEntry">The found entry.</param>
        /// <returns></returns>
        private IPreferenceEntry BuildPreferenceEntry(IPreference parent, XElement foundEntry)
        {
            var name = XElementHelper.ReadStringValue(foundEntry, "name");
            var friendlyName = XElementHelper.ReadStringValue(foundEntry, "friendlyName");
            var description = XElementHelper.ReadStringValue(foundEntry, "description", false);
            var isSelectedByDefault = XElementHelper.ReadBoolValue(foundEntry, "isDefault", false);

            if (isSelectedByDefault && parent.HasDirectlyEditableValue)
            {
                /*
                 * This preference has both a list of pre-defined entries - one of which are selected by default - 
                 * and a default value specified in the config file. There is really no obvious way to know which
                 * the user actually wanted to be the default value, so log this as a warning, and let the 
                 * pre-defined option override the directly specified default value.
                 */

                parent.Value = name;
            }
            
            var entry = new PreferenceEntry(name, friendlyName, description, parent) { IsSelected = isSelectedByDefault };

            return entry;
        }

        #endregion

        #region [ Game Configuration/Folder Retrieval ]

        /// <summary>
        /// Reads the game configuration for a particular xml node name.
        /// </summary>
        /// <param name="xmlNodeName">Name of the XML node.</param>
        /// <returns></returns>
        private IList<GameConfiguration> ReadGameConfig(string xmlNodeName)
        {
            IList<GameConfiguration> gameConfigurations = new List<GameConfiguration>();

            // Read file to memory
            var config = XmlHelper.ReadXmlFile("Configuration.xml");

            if (config == null)
            {
                return null;
            }

            // Read the games. Most likely only one (source or target), but the code here supports multiple
            var foundGames = config.Descendants(xmlNodeName);

            // For each game, read the various properties we need, and store the result in a new GameConfiguration object
            foreach (var game in foundGames)
            {
                // Save game related
                var saveGameFolderTypeAsString = XElementHelper.ReadStringValue(game, "defaultSaveGameLocationType");
                var saveGameFolderType = saveGameFolderTypeAsString.Equals(RelativeFolderLocationRoot.SteamFolder.ToString()) ? RelativeFolderLocationRoot.SteamFolder : RelativeFolderLocationRoot.WindowsUsersFolder;
                var saveGameExtension = XElementHelper.ReadStringValue(game, "saveGameExtension");

                // Installation directory related
                var steamId = XElementHelper.ReadStringValue(game, "steamId");
                var installationFolder = this.GetSteamInstallationFolder(steamId);
                var configurationFileDirectoryTagName = XElementHelper.ReadStringValue(game, "configurationFileDirectoryTagName");
                
                // Mod related
                var defaultModFolderLocationTypeAsString = XElementHelper.ReadStringValue(game, "defaultModFolderLocationType", false);
                var defaultModFolderLocationType = defaultModFolderLocationTypeAsString.Equals(RelativeFolderLocationRoot.SteamFolder.ToString()) ? RelativeFolderLocationRoot.SteamFolder : RelativeFolderLocationRoot.WindowsUsersFolder;
                var configurationFileModDirectoryTagName = XElementHelper.ReadStringValue(game, "configurationFileModDirectoryTagName", false);
                var currentModTagName = XElementHelper.ReadStringValue(game, "currentModTagName", false);

                //if (defaultModFolderLocationType == RelativeFolderLocationRoot.SteamFolder)
                //{
                //    this.options.Logger.AddLogEntry(new LogEntry("The \"defaultModFolderLocationType\" tag cannot have the value \"SteamFolder\". This value isn't supported in the frontend yet.", LogEntrySeverity.Error, LogEntrySource.UI));
                //}

                var supportedModsAsString = game.Descendants("supportedMod");

                var gameConfig = new GameConfiguration()
                {
                    Name = XElementHelper.ReadStringValue(game, "name"),
                    FriendlyName = XElementHelper.ReadStringValue(game, "friendlyName"),
                    SaveGamePath = (saveGameFolderType == RelativeFolderLocationRoot.SteamFolder ? installationFolder : this.GetUsersFolder()) + XElementHelper.ReadStringValue(game, "defaultSaveGameSubLocation"),
                    SteamId = steamId,
                    ConfigurationFileDirectoryTagName = configurationFileDirectoryTagName,
                    SaveGameExtension = saveGameExtension,
                    ConfigurationFileModDirectoryTagName = configurationFileModDirectoryTagName,
                    CurrentModTagName = currentModTagName,
                    ModPath = (defaultModFolderLocationType == RelativeFolderLocationRoot.SteamFolder ? installationFolder : GetUsersFolder()) + XElementHelper.ReadStringValue(game, "defaultModFolderLocation", false)
                };

                // Dummy item so that the user can undo selecting a mod
                var dummyMod = new SupportedMod() { Name = "No mod", IsDummyItem = true };
                gameConfig.SupportedMods.Add(dummyMod);
                gameConfig.CurrentMod = dummyMod;

                // Add proper mods
                if (supportedModsAsString.Count() > 0)
                {
                    supportedModsAsString.ForEach(m => gameConfig.SupportedMods.Add(new SupportedMod() { Name = XElementHelper.ReadStringValue(m, "modName") }));
                }

                gameConfigurations.Add(gameConfig);
            }

            return gameConfigurations;
        }

        private string GetUsersFolder()
        {
            return Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
        }

        /// <summary>
        /// Attempts to locate the folder where a steam game is installed.
        /// There might be cleaner ways of doing this, but this seems to do the job for EU3 at least.
        /// </summary>
        /// <param name="steamId">The steam Id of the game we want to look up. To find the steam id of a game, install the game via steam, 
        /// then create a shortcut on your desktop (from within steam), , show properties for the shortcut, 
        /// and get the number at the end of the URL. Ex: "steam://rungameid/25800"</param>
        /// <returns>The path to where the game is installed</returns>
        private string GetSteamInstallationFolder(string steamId)
        {
            // If installed via Steam, find game installation path.
            // The Easiest way I've found so far is to look at the uninstall keys. 
            if (!string.IsNullOrEmpty(steamId))
            {
                RegistryKey regKey = Registry.LocalMachine.OpenSubKey(@"SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\Steam App " + steamId);

                if (regKey != null)
                {
                    string steamInstallationPath = regKey.GetValue("InstallLocation").ToString();

                    if (!String.IsNullOrEmpty(steamInstallationPath))
                    {
                        if (Directory.Exists(steamInstallationPath))
                        {
                            //this.options.Logger.AddLogEntry(new LogEntry("Located Steam game files: " + steamInstallationPath, LogEntrySeverity.Info, LogEntrySource.UI));
                            return steamInstallationPath;
                        }
                    }
                }
            }

            return string.Empty;
        }

        ////private void FindGameInstallationPath(GameConfiguration gameConfiguration)
        ////{
        ////    // Could not find anything in the registry indicating whether the game uses
        ////    // the windows user documents folder for storing saves, mods, etc. 
        ////    if (!String.IsNullOrEmpty(gameConfiguration.CustomWindowsUserDocumentsSubFolder))
        ////    {
        ////        string windowsUserDocumentsFolder = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
        ////        string combinedPath = windowsUserDocumentsFolder + gameConfiguration.CustomWindowsUserDocumentsSubFolder;

        ////        if (Directory.Exists(combinedPath))
        ////        {
        ////            gameConfiguration.InstallationPath = combinedPath;
        ////            this.LogItems.Add(new LogEntry("Added custom file location: " + combinedPath));
        ////        }
        ////    }
        ////}

        #endregion

        #region [ Helper Methods ]

        /// <summary>
        /// Shows an (helpful?) error message if something goes wrong during parsing.
        /// </summary>
        /// <param name="xmlFileName">Name of the XML file.</param>
        /// <param name="exception">The exception.</param>
        private void ShowMalformedOrMissingXMLElementError(string xmlFileName, XMLParseExceptionBase exception)
        {
            var lineInfo = FindExpectedLineNumber(exception.ParentElement);

            var message = "There was an error reading " + xmlFileName;

            if (lineInfo != null)
            {
                message += ". The element starting at " + lineInfo.Item1 + ", position: " + lineInfo.Item2 + " seems to be missing subitem named " + exception.PropertyName + ".";
            }
            else
            {
                message += "; the element named " + exception.PropertyName + " couldn't be properly read.";
            }
                        
            MessageBox.Show(message, "Configuration file error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        /// <summary>
        /// Finds the expected line number. Useful for the user if something went wrong during parsing
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <returns></returns>
        private Tuple<int, int> FindExpectedLineNumber(XElement parentElement)
        {
            IXmlLineInfo info = parentElement;

            return info.HasLineInfo() ? new Tuple<int, int>(info.LineNumber, info.LinePosition) : null;
        }

        #endregion

        #endregion
        #endregion

    }
}
