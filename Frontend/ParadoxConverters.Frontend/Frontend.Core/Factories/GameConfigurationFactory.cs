using System.Linq;
using System.Xml.Linq;
using Caliburn.Micro;
using Frontend.Core.Helpers;
using Frontend.Core.Model;

namespace Frontend.Core.Factories
{
    public class GameConfigurationFactory : FactoryBase
    {
        ///// <summary>
        ///// The required folder factory
        ///// </summary>
        //private RequiredFolderFactory requiredFolderFactory;

        ///// <summary>
        ///// The required item file factory
        ///// </summary>
        //private RequiredFileFactory requiredFileFactory;

        //private XDocument config;

        public GameConfigurationFactory(IEventAggregator eventAggregator)
            : base(eventAggregator, "gameConfiguration")
        {
        }

        ///// <summary>
        ///// The required folder factory self-resolving property
        ///// </summary>
        //protected RequiredFolderFactory RequiredFolderFactory
        //{
        //    get
        //    {
        //        return this.requiredFolderFactory ?? (this.requiredFolderFactory = new RequiredFolderFactory(this.EventAggregator));
        //    }
        //}

        ///// <summary>
        ///// The required file factory self-resolving property
        ///// </summary>
        //protected RequiredFileFactory RequiredFileFactory
        //{
        //    get
        //    {
        //        return this.requiredFileFactory ?? (this.requiredFileFactory = new RequiredFileFactory(this.EventAggregator));
        //    }
        //}

        //protected override void OnConfigLoaded(XDocument config)
        //{
        //    base.OnConfigLoaded(config);
        //    this.config = config;
        //}

        protected override T OnBuildElement<T>(XElement element)
        {
            var name = XElementHelper.ReadStringValue(element, "name");
            var friendlyName = XElementHelper.ReadStringValue(element, "friendlyName");
            //var steamId = XElementHelper.ReadStringValue(element, "steamId");

            //var requiredFolders = this.RequiredFolderFactory.BuildConfiguration<IRequiredFolder>(this.config);
            //var requiredFiles = this.RequiredFileFactory.BuildConfiguration<IRequiredFile>(this.config);

            //var requiredFilesAndFolders = requiredFiles.Union(requiredFolders);

            // Installation directory related
            //var installationFolder = SteamHelper.GetSteamInstallationFolder(this.EventAggregator, steamId);
            //var installationDirectoryTagName = XElementHelper.ReadStringValue(element, "installationDirectoryTagName");
            //var iconFile = XElementHelper.ReadStringValue(element, "iconFile", false);

            // Save game related
            //var saveGameFolderTypeAsString = XElementHelper.ReadStringValue(element, "defaultSaveGameLocationType");
            //var saveGameFolderType = saveGameFolderTypeAsString.Equals(RelativeFolderLocationRoot.SteamFolder.ToString()) ? RelativeFolderLocationRoot.SteamFolder : RelativeFolderLocationRoot.WindowsUsersFolder;
            //var saveGameExtension = XElementHelper.ReadStringValue(element, "saveGameExtension");
            //var saveGameLocation = (saveGameFolderType == RelativeFolderLocationRoot.SteamFolder ? installationFolder : DirectoryHelper.GetUsersFolder()) + XElementHelper.ReadStringValue(element, "defaultSaveGameSubLocation");

            // Mod related
            //var defaultModFolderLocationTypeAsString = XElementHelper.ReadStringValue(element, "defaultModFolderLocationType", false);
            //var defaultModFolderLocationType = defaultModFolderLocationTypeAsString.Equals(RelativeFolderLocationRoot.SteamFolder.ToString()) ? RelativeFolderLocationRoot.SteamFolder : RelativeFolderLocationRoot.WindowsUsersFolder;
            //var modDirectoryTagName = XElementHelper.ReadStringValue(element, "modDirectoryTagName", false);
            var currentModTagName = XElementHelper.ReadStringValue(element, "currentModTagName", false);
            //var absoluteModPath = (defaultModFolderLocationType == RelativeFolderLocationRoot.SteamFolder ? installationFolder : DirectoryHelper.GetUsersFolder()) + XElementHelper.ReadStringValue(element, "defaultModFolderLocation", false);
            var supportedModsAsString = element.Descendants("supportedMod");

            // Temp directory
            //var tempDirectoryLocationTypeAsString = XElementHelper.ReadStringValue(element, "defaultTempDirectoryLocationType", false);
            //var tempDirectoryLocationType = tempDirectoryLocationTypeAsString.Equals(RelativeFolderLocationRoot.SteamFolder.ToString()) ? RelativeFolderLocationRoot.SteamFolder : RelativeFolderLocationRoot.WindowsUsersFolder;
            //var tempDirectoryAbsolutePath = (tempDirectoryLocationType == RelativeFolderLocationRoot.SteamFolder ? installationFolder : DirectoryHelper.GetUsersFolder()) + XElementHelper.ReadStringValue(element, "defaultTempFolderLocation", false);
            //var tempDirectoryTagName = XElementHelper.ReadStringValue(element, "tempDirectoryTagName", false);

            var gameConfig = new GameConfiguration
            {
                Name = name,
                FriendlyName = friendlyName,
                //SteamId = steamId,
                //SaveGameExtension = saveGameExtension,
                //AbsoluteInstallationPath = installationFolder,
                //InstallationDirectoryTagName = installationDirectoryTagName,
                //ModDirectoryTagName = modDirectoryTagName,
                //AbsoluteSaveGamePath = saveGameLocation,
                CurrentModTagName = currentModTagName
                //AbsoluteModPath = absoluteModPath/*,
                //AbsoluteTempDirectoryPath = tempDirectoryAbsolutePath,
                //TempDirectoryTagName = tempDirectoryTagName
            };

            // Dummy item so that the user can undo selecting a mod
            var dummyMod = new Mod {Name = "No mod", IsDummyItem = true};
            gameConfig.SupportedMods.Add(dummyMod);
            gameConfig.CurrentMod = dummyMod;

            // Add proper mods
            if (supportedModsAsString.Count() > 0)
            {
                supportedModsAsString.ForEach(
                    m => gameConfig.SupportedMods.Add(new Mod {Name = XElementHelper.ReadStringValue(m, "modName")}));
            }

            return gameConfig as T;
        }
    }
}