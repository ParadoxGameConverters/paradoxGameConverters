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
            var saveGameLocation = (saveGameFolderType == RelativeFolderLocationRoot.SteamFolder ? installationFolder : this.GetUsersFolder()) +XElementHelper.ReadStringValue(element, "defaultSaveGameSubLocation");


            return new GameConfiguration()
            {
                Name = name,
                FriendlyName = friendlyName,
                SteamId = steamId,
                SaveGameExtension = saveGameExtension,
                AbsoluteInstallationPath = installationFolder,
                ConfigurationFileDirectoryTagName = configurationFileDirectoryTagName,
                AbsoluteSaveGamePath =  saveGameLocation
            } as T;
        }

        private string GetUsersFolder()
        {
            return Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
        }
    }
}
