using Caliburn.Micro;
using Frontend.Core.Logging;
using Frontend.Core.Model.Interfaces;

namespace Frontend.Core.Model
{
    public class ConverterSettings : PropertyChangedBase, IConverterSettings
    {
        private bool isSelected;
        private IEventAggregator eventAggregator;

        public ConverterSettings(IEventAggregator eventAggregator)
        {
            this.eventAggregator = eventAggregator;
        }

        public string Name { get; set; }
        public string FriendlyName { get; set; }
        public string DefaultConfigurationFile { get; set; }
        public string UserConfigurationFile { get; set; }
        public string ConverterExeName { get; set; }
        public IGameConfiguration SourceGame { get; set; }
        public IGameConfiguration TargetGame { get; set; }

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
                    this.eventAggregator.PublishOnUIThread(new LogEntry("Using configuration file " + this.DefaultConfigurationFile, LogEntrySeverity.Info, LogEntrySource.UI, this.DefaultConfigurationFile));
                }
            }
        }
    }
}
