using System;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Xml.Linq;
using Caliburn.Micro;
using Frontend.Core.Helpers;
using Frontend.Core.Logging;
using Frontend.Core.Model;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.Model.Paths.Interfaces;

namespace Frontend.Core.Factories
{
    public class ConverterSettingsFactory : FactoryBase
    {
        private XDocument config;

        /// <summary>
        ///     The Game configuration factory
        /// </summary>
        private GameConfigurationFactory gameConfigurationFactory;

        /// <summary>
        ///     The list of game configurations
        /// </summary>
        private ObservableCollection<IGameConfiguration> gameConfigurations;

        /// <summary>
        ///     The preference category factory
        /// </summary>
        private PreferenceCategoryFactory preferenceCategoryFactory;

        /// <summary>
        ///     The relative path to the game configuration config file
        /// </summary>
        private string relativeGameConfigurationPath;

        /// <summary>
        ///     The required item file factory
        /// </summary>
        private RequiredFileFactory requiredFileFactory;

        /// <summary>
        ///     The required item (folder, file) factory
        /// </summary>
        private RequiredFolderFactory requiredFolderFactory;

        /// <summary>
        ///     Initializes a new instance of the ConverterSettingsFactory
        /// </summary>
        /// <param name="eventAggregator">The event aggregator</param>
        public ConverterSettingsFactory(IEventAggregator eventAggregator)
            : base(eventAggregator, "converter")
        {
        }

        /// <summary>
        ///     The game configuration factory self-resolving property
        /// </summary>
        protected GameConfigurationFactory GameConfigurationFactory
        {
            get
            {
                return gameConfigurationFactory ??
                       (gameConfigurationFactory = new GameConfigurationFactory(EventAggregator));
            }
        }

        /// <summary>
        ///     The game configuration factory self-resolving property
        /// </summary>
        protected PreferenceCategoryFactory PreferenceCategoryFactory
        {
            get
            {
                return preferenceCategoryFactory ??
                       (preferenceCategoryFactory = new PreferenceCategoryFactory(EventAggregator));
            }
        }

        /// <summary>
        ///     Helper property for turning the relative game configuration path into an absolute path.
        ///     What was I thinking?
        /// </summary>
        protected string AbsoluteGameConfigurationPath
        {
            get { return Path.Combine(Environment.CurrentDirectory, relativeGameConfigurationPath); }
        }

        /// <summary>
        ///     Self-building list of gameconfiguration objects.
        ///     Requires the AbsoluteGameConfiguration path property to make sense.
        ///     If not, returns an empty collection and complains to the logger.
        ///     If the configuration file exists on the AbsoluteGameConfigurationPath,
        ///     this collection gets built usind the GameConfigurationFactory.
        /// </summary>
        protected ObservableCollection<IGameConfiguration> GameConfigurations
        {
            get
            {
                if (gameConfigurations == null)
                {
                    if (!File.Exists(AbsoluteGameConfigurationPath))
                    {
                        gameConfigurations = new ObservableCollection<IGameConfiguration>();
                        EventAggregator.PublishOnUIThread(
                            new LogEntry("Could not find game configuration file at: " + AbsoluteGameConfigurationPath,
                                LogEntrySeverity.Error, LogEntrySource.UI, AbsoluteGameConfigurationPath));
                    }
                    else
                    {
                        gameConfigurations =
                            GameConfigurationFactory.BuildModels<IGameConfiguration>(AbsoluteGameConfigurationPath);
                    }
                }

                return gameConfigurations;
            }
        }

        /// <summary>
        ///     The required item factory self-resolving property
        /// </summary>
        protected RequiredFolderFactory RequiredFolderFactory
        {
            get
            {
                return requiredFolderFactory ?? (requiredFolderFactory = new RequiredFolderFactory(EventAggregator));
            }
        }

        /// <summary>
        ///     The required file factory self-resolving property
        /// </summary>
        protected RequiredFileFactory RequiredFileFactory
        {
            get { return requiredFileFactory ?? (requiredFileFactory = new RequiredFileFactory(EventAggregator)); }
        }

        /// <summary>
        ///     TODO:
        /// </summary>
        /// <param name="config"></param>
        protected override void OnConfigLoaded(XDocument config)
        {
            this.config = config;
            relativeGameConfigurationPath = XElementHelper.ReadStringValue(config.Descendants("configuration").First(),
                "gameConfigurationFile");
        }

        /// <summary>
        ///     Builds the ConverterSettings object given by the xml element (XElement)
        /// </summary>
        /// <typeparam name="T">The type of object to create. In this case, it'll be ConverterSettings</typeparam>
        /// <param name="element">The xml node to generate the ConverterSettings object from</param>
        /// <returns>The generated convertersettings object</returns>
        protected override T OnBuildElement<T>(XElement element)
        {
            var name = XElementHelper.ReadStringValue(element, "name");
            var friendlyName = XElementHelper.ReadStringValue(element, "friendlyName");
            var defaultConfigurationFile = XElementHelper.ReadStringValue(element, "defaultConfigurationFile");
            //var converterExeName = XElementHelper.ReadStringValue(element, "converterExeName");
            //var userConfigurationFile = XElementHelper.ReadStringValue(element, "userConfigurationFile");
            var additionalInformation = XElementHelper.ReadStringValue(element, "informationText", false);

            // Build game configuration models
            var sourceGameName = XElementHelper.ReadStringValue(element, "sourceGame");
            var targetGameName = XElementHelper.ReadStringValue(element, "targetGame");
            var sourceGame = GameConfigurations.FirstOrDefault(g => g.Name.Equals(sourceGameName));
            var targetGame = GameConfigurations.FirstOrDefault(g => g.Name.Equals(targetGameName));

            var requiredFolders = RequiredFolderFactory.BuildConfiguration<IRequiredFolder>(config);
            var requiredFiles = RequiredFileFactory.BuildConfiguration<IRequiredFile>(config);

            // Native export directory.
            //var nativeExportDirectory = XElementHelper.ReadStringValue(element, "nativeParadoxExportDirectory", false);
            //var nativeParadoxExportDirectoryLocationTypeAsString = XElementHelper.ReadStringValue(element, "nativeParadoxExportDirectoryLocationType", false);
            //var nativeParadoxExportDirectoryLocationType = nativeParadoxExportDirectoryLocationTypeAsString.Equals(RelativeFolderLocationRoot.SteamFolder.ToString()) ? RelativeFolderLocationRoot.SteamFolder : RelativeFolderLocationRoot.WindowsUsersFolder;
            //string nativeExportDirectoryTag = XElementHelper.ReadStringValue(element, "nativeParadoxExportDirectoryTag", false);

            //string nativeExportDirectoryAbsolutePath = string.Empty;
            //if (nativeParadoxExportDirectoryLocationType.Equals(RelativeFolderLocationRoot.WindowsUsersFolder))
            //{
            //    nativeExportDirectoryAbsolutePath = DirectoryHelper.GetUsersFolder() + XElementHelper.ReadStringValue(element, "nativeParadoxExportDirectory", false);                
            //}
            //else
            //{
            //    //HACK: This is bad, and needs fixing.
            //    throw new NotSupportedException("The native export directory cannot be a steam subfolder.");
            //}

            var errorMessage = "Could not find game configuration for {0}. Could not find game in " +
                               AbsoluteGameConfigurationPath + " with name {1}. ";

            // Build preference categories
            var categories = PreferenceCategoryFactory.BuildModels<IPreferenceCategory>(defaultConfigurationFile);

            if (sourceGame == null)
            {
                EventAggregator.PublishOnUIThread(
                    new LogEntry(string.Format(errorMessage, "source game", sourceGameName), LogEntrySeverity.Error,
                        LogEntrySource.UI, AbsoluteGameConfigurationPath));
            }

            if (targetGame == null)
            {
                EventAggregator.PublishOnUIThread(
                    new LogEntry(string.Format(errorMessage, "target game", targetGameName), LogEntrySeverity.Error,
                        LogEntrySource.UI, AbsoluteGameConfigurationPath));
            }

            //var relativeConverterPath = XElementHelper.ReadStringValue(element, "subfolderName");

            var settings = new ConverterSettings(EventAggregator)
            {
                Name = name,
                FriendlyName = friendlyName,
                DefaultConfigurationFile = Path.Combine(Environment.CurrentDirectory, defaultConfigurationFile),
                //ConverterExeName = converterExeName,
                SourceGame = sourceGame,
                TargetGame = targetGame,
                //AbsoluteConverterPath = Path.Combine(Environment.CurrentDirectory, relativeConverterPath),
                /*NativeParadoxExportDirectoryTag = nativeExportDirectoryTag,
                NativeParadoxExportDirectory = nativeExportDirectoryAbsolutePath,*/
                //UserConfigurationFile = userConfigurationFile 
                Categories = categories,
                AdditionalInformation = additionalInformation
            };

            requiredFolders.ForEach(f => settings.RequiredItems.Add(f));
            requiredFiles.ForEach(f => settings.RequiredItems.Add(f));

            return settings as T;
        }
    }
}