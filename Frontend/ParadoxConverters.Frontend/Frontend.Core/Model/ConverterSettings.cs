using Caliburn.Micro;
using Frontend.Core.Logging;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.Model.Paths.Interfaces;
using System.Collections.Generic;
using System.Linq;

namespace Frontend.Core.Model
{
    public class ConverterSettings : PropertyChangedBase, IConverterSettings
    {
        private bool isSelected;
        private bool useConverterMod;
        private IEventAggregator eventAggregator;
        private IList<IRequiredItemBase> requiredItems;

        public ConverterSettings(IEventAggregator eventAggregator)
        {
            this.eventAggregator = eventAggregator;
        }

        public string Name { get; set; }
        public string FriendlyName { get; set; }
        public string DefaultConfigurationFile { get; set; }
        public string UserConfigurationFile { get; set; }
        public IGameConfiguration SourceGame { get; set; }
        public IGameConfiguration TargetGame { get; set; }
        public IList<IPreferenceCategory> Categories { get; set; }
        
        public string AdditionalInformation { get; set; }

        public IList<IRequiredItemBase> RequiredItems
        {
            get
            {
                if (this.requiredItems == null)
                {
                    this.requiredItems = new List<IRequiredItemBase>();
                }

                return this.requiredItems;
            }
        }

        /// <summary>
        /// Gets or sets a value indicating whether [is selected].
        /// </summary>
        /// <value>
        ///   <c>true</c> if [is selected]; otherwise, <c>false</c>.
        /// </value>
        public bool IsSelected
        {
            get
            {
                return this.isSelected;
            }

            set
            {
                if (this.isSelected == value)
                {
                    return;
                }

                this.isSelected = value;
                this.NotifyOfPropertyChange(() => this.IsSelected);

                if (value)
                {
                    this.eventAggregator.PublishOnUIThread(new LogEntry("Using configuration file", LogEntrySeverity.Info, LogEntrySource.UI, this.DefaultConfigurationFile));
                    this.eventAggregator.PublishOnUIThread((IConverterSettings)this);
                }
            }
        }

        /// <summary>
        /// Gets or sets the source save game.
        /// </summary>
        /// <value>
        /// The source save game.
        /// </value>
        public IRequiredFile AbsoluteSourceSaveGame
        {
            get
            {
                //TODO: This is bad and I should feel bad
                return (IRequiredFile)this.RequiredItems.First(f => f.FriendlyName.Equals("Savegame"));
            }
        }

        /// <summary>
        /// Gets or sets the converter path.
        /// </summary>
        /// <value>
        /// The converter.
        /// </value>
        public IRequiredFile AbsoluteConverter
        {
            get
            {
                //TODO: This is bad and I should feel bad
                return (IRequiredFile)this.RequiredItems.First(f => f.FriendlyName.Equals("Converter .exe"));
            }
        }

        /// <summary>
        /// Gets or sets a value indicating whether to use the converter mod.
        /// </summary>
        /// <value>
        ///   <c>true</c> if [use converter mod]; otherwise, <c>false</c>.
        /// </value>
        public bool UseConverterMod
        {
            get
            {
                return this.useConverterMod;
            }

            set
            {
                if (this.useConverterMod == value)
                {
                    return;
                }

                this.useConverterMod = value;
                this.NotifyOfPropertyChange(() => this.UseConverterMod);
            }
        }
    }
}
