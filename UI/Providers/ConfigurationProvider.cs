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
    public class ConfigurationProvider
    {
        #region [ Fields ]

        private IList<GameConfiguration> sourceGames;
        private IList<GameConfiguration> targetGames;
        private IList<PreferenceCategory> preferenceCategories;

        #endregion

        #region [ Constructor ]

        public ConfigurationProvider()
        {
        }

        #endregion

        #region [ Properties ]

        #region [ Public Properties ]

        public IList<GameConfiguration> SourceGames
        {
            get
            {
                return this.sourceGames ?? (this.sourceGames = this.ReadGameConfig("sourceGameConfigurations"));
            }
        }

        public IList<GameConfiguration> TargetGames
        {
            get
            {
                return this.targetGames ?? (this.targetGames = this.ReadGameConfig("targetGameConfigurations"));
            }
        }

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
                var foundCategories = config.Descendants("category");
                
                foreach (var foundCategory in foundCategories)
                {
                    var category = new PreferenceCategory();
                    category.FriendlyName = XElementHelper.ReadStringValue(foundCategory, "friendlyName");
                    categories.Add(category);

                    var foundPreferences = XElementHelper.ReadEnumerable(foundCategory, "preference");

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
                        
                        category.Preferences.Add(preference);

                        var foundEntries = XElementHelper.ReadEnumerable(foundPreference, "entryOption", false);
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
                MessageBox.Show(ex.Message + " - " + ex.InnerException, "Configuration file error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            return categories;
        }

        #endregion

        #region [ Preference Entry Construction ]

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

        private IList<GameConfiguration> ReadGameConfig(string xmlNodeName)
        {
            IList<GameConfiguration> gameConfigurations = new List<GameConfiguration>();

            var config = XmlHelper.ReadXmlFile("Configuration.xml");

            if (config == null)
            {
                return null;
            }

            var foundGames = config.Descendants(xmlNodeName);

            foreach (var game in foundGames)
            {
                var saveGameFolderType = XElementHelper.ReadStringValue(game, "defaultSaveGameLocationType");
                var type = saveGameFolderType.Equals(DefaultSaveGameLocationType.SteamFolder.ToString()) ? DefaultSaveGameLocationType.SteamFolder : DefaultSaveGameLocationType.WindowsUsersFolder;
                var steamId = XElementHelper.ReadStringValue(game, "steamId");
                var installationFolder = this.GetSteamInstallationFolder(steamId);
                var configurationFileDirectoryTagName = XElementHelper.ReadStringValue(game, "configurationFileDirectoryTagName");

                gameConfigurations.Add(new GameConfiguration()
                {
                    Name = XElementHelper.ReadStringValue(game, "name"),
                    FriendlyName = XElementHelper.ReadStringValue(game, "friendlyName"),
                    SaveGamePath = (type == DefaultSaveGameLocationType.SteamFolder ? installationFolder : this.GetUsersFolder()) + XElementHelper.ReadStringValue(game, "defaultSaveGameSubLocation"),
                    SteamId = steamId,
                    ConfigurationFileDirectoryTagName = configurationFileDirectoryTagName
                });
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
                            //this.LogItems.Add(new LogEntry("Located Steam game files: " + steamInstallationPath));
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
