using Caliburn.Micro;
using Frontend.Core.Factories;
using Frontend.Core.Logging;
using Frontend.Core.Model;
using Frontend.Core.ViewModels.Interfaces;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.ViewModels
{
    public class WelcomeViewModel : StepViewModelBase, IWelcomeViewModel
    {
        private ObservableCollection<ConverterSettings> supportedConverters;
        private ConverterSettingsFactory settingsFactory;

        public WelcomeViewModel(IEventAggregator eventAggregator)
            : base(eventAggregator)
        {
        }

        protected ConverterSettingsFactory SettingsFactory
        {
            get
            {
                return this.settingsFactory ?? (this.settingsFactory = new ConverterSettingsFactory(this.EventAggregator));
            }
        }

        public ObservableCollection<ConverterSettings> SupportedConverters
        {
            get
            {
                var currentDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
                return this.supportedConverters ?? (this.supportedConverters = 
                    this.SettingsFactory.BuildModels<ConverterSettings>(Path.Combine(currentDirectory, "Configuration/SupportedConvertersDefault.xml")));
            }
        }

        protected override void OnLoading(object parameter)
        {
            if (this.EventAggregator.HandlerExistsFor(typeof(LogEntry)))
            {
                this.EventAggregator.Publish(new LogEntry("Application starting", LogEntrySeverity.Info, LogEntrySource.UI), null);
            }
        }
    }
}
