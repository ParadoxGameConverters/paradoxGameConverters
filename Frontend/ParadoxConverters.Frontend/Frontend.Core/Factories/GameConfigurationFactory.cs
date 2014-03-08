using Caliburn.Micro;
using Frontend.Core.Helpers;
using Frontend.Core.Model;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace Frontend.Core.Factories
{
    public class GameConfigurationFactory : FactoryBase
    {
        public GameConfigurationFactory(IEventAggregator eventAggregator)
            : base(eventAggregator, "gameConfiguration")
        {
        }

        protected override T OnBuildElement<T>(XElement element)
        {
            var name = XElementHelper.ReadStringValue(element, "name");
            var friendlyName = XElementHelper.ReadStringValue(element, "friendlyName");
            var steamId = XElementHelper.ReadStringValue(element, "steamId");

            // Installation directory related
            var installationFolder = SteamHelper.GetSteamInstallationFolder(this.EventAggregator, steamId);
            var configurationFileDirectoryTagName = XElementHelper.ReadStringValue(element, "configurationFileDirectoryTagName");
            //var iconFile = XElementHelper.ReadStringValue(element, "iconFile", false);

            // Save game related
            var saveGameFolderTypeAsString = XElementHelper.ReadStringValue(element, "defaultSaveGameLocationType");
            var saveGameFolderType = saveGameFolderTypeAsString.Equals(RelativeFolderLocationRoot.SteamFolder.ToString()) ? RelativeFolderLocationRoot.SteamFolder : RelativeFolderLocationRoot.WindowsUsersFolder;
            var saveGameExtension = XElementHelper.ReadStringValue(element, "saveGameExtension");
            var saveGameLocation = (saveGameFolderType == RelativeFolderLocationRoot.SteamFolder ? installationFolder : this.GetUsersFolder()) + XElementHelper.ReadStringValue(element, "defaultSaveGameSubLocation");

            // Mod related
            var defaultModFolderLocationTypeAsString = XElementHelper.ReadStringValue(element, "defaultModFolderLocationType", false);
            var defaultModFolderLocationType = defaultModFolderLocationTypeAsString.Equals(RelativeFolderLocationRoot.SteamFolder.ToString()) ? RelativeFolderLocationRoot.SteamFolder : RelativeFolderLocationRoot.WindowsUsersFolder;
            var configurationFileModDirectoryTagName = XElementHelper.ReadStringValue(element, "configurationFileModDirectoryTagName", false);
            var currentModTagName = XElementHelper.ReadStringValue(element, "currentModTagName", false);
            var absoluteModPath = (defaultModFolderLocationType == RelativeFolderLocationRoot.SteamFolder ? installationFolder : GetUsersFolder()) + XElementHelper.ReadStringValue(element, "defaultModFolderLocation", false);
            var supportedModsAsString = element.Descendants("supportedMod");

            var gameConfig = new GameConfiguration()
            {
                Name = name,
                FriendlyName = friendlyName,
                SteamId = steamId,
                SaveGameExtension = saveGameExtension,
                AbsoluteInstallationPath = installationFolder,
                ConfigurationFileDirectoryTagName = configurationFileDirectoryTagName,
                ConfigurationFileModDirectoryTagName = configurationFileModDirectoryTagName,
                AbsoluteSaveGamePath = saveGameLocation,
                CurrentModTagName = currentModTagName,
                AbsoluteModPath = absoluteModPath
            };

            // Dummy item so that the user can undo selecting a mod
            var dummyMod = new Mod() { Name = "No mod", IsDummyItem = true };
            gameConfig.SupportedMods.Add(dummyMod);
            gameConfig.CurrentMod = dummyMod;

            // Add proper mods
            if (supportedModsAsString.Count() > 0)
            {
                supportedModsAsString.ForEach(m => gameConfig.SupportedMods.Add(new Mod() { Name = XElementHelper.ReadStringValue(m, "modName") }));
            }

            return gameConfig as T;
        }

        private string GetUsersFolder()
        {
            return Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
        }
    }
}
