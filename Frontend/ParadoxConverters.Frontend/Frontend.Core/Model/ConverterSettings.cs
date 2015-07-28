using System.Collections.Generic;
using System.Linq;
using Caliburn.Micro;
using Frontend.Core.Logging;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.Model.Paths.Interfaces;

namespace Frontend.Core.Model
{
    public class ConverterSettings : PropertyChangedBase, IConverterSettings
    {
        private readonly IEventAggregator eventAggregator;
        private bool isSelected;
        private IList<IRequiredItemBase> requiredItems;
        private bool useConverterMod;

        public ConverterSettings(IEventAggregator eventAggregator)
        {
            this.eventAggregator = eventAggregator;
        }

        public string AdditionalInformation { get; set; }
        public string Name { get; set; }
        public string FriendlyName { get; set; }
        public string DefaultConfigurationFile { get; set; }
        public string UserConfigurationFile { get; set; }
        public IGameConfiguration SourceGame { get; set; }
        public IGameConfiguration TargetGame { get; set; }
        public IList<IPreferenceCategory> Categories { get; set; }

        public IList<IRequiredItemBase> RequiredItems
        {
            get
            {
                if (requiredItems == null)
                {
                    requiredItems = new List<IRequiredItemBase>();
                }

                return requiredItems;
            }
        }

        /// <summary>
        ///     Gets or sets a value indicating whether [is selected].
        /// </summary>
        /// <value>
        ///     <c>true</c> if [is selected]; otherwise, <c>false</c>.
        /// </value>
        public bool IsSelected
        {
            get { return isSelected; }

            set
            {
                if (isSelected == value)
                {
                    return;
                }

                isSelected = value;
                NotifyOfPropertyChange(() => IsSelected);

                if (value)
                {
                    eventAggregator.PublishOnUIThread(new LogEntry("Using configuration file", LogEntrySeverity.Info,
                        LogEntrySource.UI, DefaultConfigurationFile));
                    eventAggregator.PublishOnUIThread(this);
                }
            }
        }

        /// <summary>
        ///     Gets or sets the source save game.
        /// </summary>
        /// <value>
        ///     The source save game.
        /// </value>
        public IRequiredFile AbsoluteSourceSaveGame
        {
            get
            {
                //TODO: This is bad and I should feel bad
                return (IRequiredFile) RequiredItems.First(f => f.FriendlyName.Equals("Savegame"));
            }
        }

        /// <summary>
        ///     Gets or sets the converter path.
        /// </summary>
        /// <value>
        ///     The converter.
        /// </value>
        public IRequiredFile AbsoluteConverter
        {
            get
            {
                //TODO: This is bad and I should feel bad
                return (IRequiredFile) RequiredItems.First(f => f.FriendlyName.Equals("Converter .exe"));
            }
        }

        public IRequiredFile Faq
        {
            get { return (IRequiredFile) RequiredItems.First(f => f.InternalTagName.Equals("faq")); }
        }

        public IRequiredFile Log
        {
            get { return (IRequiredFile)RequiredItems.First(f => f.InternalTagName.Equals("log")); }
        }

        /// <summary>
        ///     Gets or sets a value indicating whether to use the converter mod.
        /// </summary>
        /// <value>
        ///     <c>true</c> if [use converter mod]; otherwise, <c>false</c>.
        /// </value>
        public bool UseConverterMod
        {
            get { return useConverterMod; }

            set
            {
                if (useConverterMod == value)
                {
                    return;
                }

                useConverterMod = value;
                NotifyOfPropertyChange(() => UseConverterMod);
            }
        }
    }
}