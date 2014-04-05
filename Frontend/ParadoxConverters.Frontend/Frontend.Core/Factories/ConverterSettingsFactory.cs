using Caliburn.Micro;
using Frontend.Core.Helpers;
using Frontend.Core.Logging;
using Frontend.Core.Model;
using Frontend.Core.Model.Interfaces;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace Frontend.Core.Factories
{
    public class ConverterSettingsFactory : FactoryBase
    {
        private GameConfigurationFactory gameConfigurationFactory;
        private PreferenceCategoryFactory preferenceCategoryFactory;
        private ObservableCollection<IGameConfiguration> gameConfigurations;
        private string relativeGameConfigurationPath;

        public ConverterSettingsFactory(IEventAggregator eventAggregator)
            : base(eventAggregator, "converter")
        {
        }

        protected GameConfigurationFactory GameConfigurationFactory
        {
            get
            {
                return this.gameConfigurationFactory ?? (this.gameConfigurationFactory = new GameConfigurationFactory(this.EventAggregator));
            }
        }

        protected PreferenceCategoryFactory PreferenceCategoryFactory
        {
            get
            {
                return this.preferenceCategoryFactory ?? (this.preferenceCategoryFactory = new PreferenceCategoryFactory(this.EventAggregator));
            }
        }

        protected string AbsoluteGameConfigurationPath
        {
            get
            {
                return Path.Combine(Environment.CurrentDirectory, relativeGameConfigurationPath);
            }
        }

        protected ObservableCollection<IGameConfiguration> GameConfigurations
        {
            get
            {
                if (this.gameConfigurations == null)
                {
                    if (!File.Exists(this.AbsoluteGameConfigurationPath))
                    {
                        this.gameConfigurations = new ObservableCollection<IGameConfiguration>();
                        this.EventAggregator.PublishOnUIThread(new LogEntry("Could not find game configuration file at: " + this.AbsoluteGameConfigurationPath, LogEntrySeverity.Error, LogEntrySource.UI, this.AbsoluteGameConfigurationPath));
                    }
                    else
                    {
                        this.gameConfigurations = this.GameConfigurationFactory.BuildModels<IGameConfiguration>(this.AbsoluteGameConfigurationPath);
                    }
                }

                return this.gameConfigurations;
            }
        }

        protected override void OnConfigLoaded(XDocument config)
        {
            this.relativeGameConfigurationPath = XElementHelper.ReadStringValue(config.Descendants("configuration").First(), "gameConfigurationFile");
        }

        protected override T OnBuildElement<T>(XElement element)
        {
            var name = XElementHelper.ReadStringValue(element, "name");
            var friendlyName = XElementHelper.ReadStringValue(element, "friendlyName");
            var defaultConfigurationFile = XElementHelper.ReadStringValue(element, "defaultConfigurationFile");
            var converterExeName = XElementHelper.ReadStringValue(element, "converterExeName");
            //var userConfigurationFile = XElementHelper.ReadStringValue(element, "userConfigurationFile");

            // Build game configuration models
            var sourceGameName = XElementHelper.ReadStringValue(element, "sourceGame");
            var targetGameName = XElementHelper.ReadStringValue(element, "targetGame");
            var sourceGame = this.GameConfigurations.FirstOrDefault(g => g.Name.Equals(sourceGameName));
            var targetGame = this.GameConfigurations.FirstOrDefault(g => g.Name.Equals(targetGameName));

            string errorMessage = "Could not find game configuration for {0}. Could not find game in " + this.AbsoluteGameConfigurationPath + " with name {1}. ";

            // Build preference categories
            var categories = this.PreferenceCategoryFactory.BuildModels<IPreferenceCategory>(defaultConfigurationFile);

            if (sourceGame == null)
            {
                this.EventAggregator.PublishOnUIThread(new LogEntry(String.Format(errorMessage, "source game", sourceGameName), LogEntrySeverity.Error, LogEntrySource.UI, this.AbsoluteGameConfigurationPath));
            }
            
            if (targetGame == null)
            {
                this.EventAggregator.PublishOnUIThread(new LogEntry(String.Format(errorMessage, "target game", targetGameName), LogEntrySeverity.Error, LogEntrySource.UI, this.AbsoluteGameConfigurationPath));
            }

            var relativeConverterPath = XElementHelper.ReadStringValue(element, "subfolderName");

            return new ConverterSettings(this.EventAggregator)
            {
                Name = name,
                FriendlyName = friendlyName,
                DefaultConfigurationFile = Path.Combine(Environment.CurrentDirectory, defaultConfigurationFile),
                ConverterExeName = converterExeName,
                SourceGame = sourceGame,
                TargetGame = targetGame,
                AbsoluteConverterPath = Path.Combine(Environment.CurrentDirectory, relativeConverterPath),
                //UserConfigurationFile = userConfigurationFile 
                Categories = categories
            } as T;
        }
    }
}
