using Caliburn.Micro;
using Frontend.Core.Logging;
using Frontend.Core.Model.Interfaces;
using System;
using System.Linq;
using System.Collections.ObjectModel;
using System.IO;
using System.Xml.Linq;
using Frontend.Core.Helpers;
using System.Reflection;

namespace Frontend.Core.Factories
{
    public abstract class FactoryBase
    {
        private IEventAggregator eventAggregator;
        private string xmlElementIdentifier;
        //Func<XElement, bool> filter;

        public FactoryBase(IEventAggregator eventAggregator, string xmlElementIdentifier, Func<XElement, bool> filter)
        {
            this.eventAggregator = eventAggregator;
            this.xmlElementIdentifier = xmlElementIdentifier;
            //this.filter = filter;
        }

        public FactoryBase(IEventAggregator eventAggregator, string xmlElementIdentifier)
            : this(eventAggregator, xmlElementIdentifier, null)
        {
        }

        protected IEventAggregator EventAggregator
        {
            get
            {
                return this.eventAggregator;
            }
        }

        public ObservableCollection<T> BuildModels<T>(string configurationFilePath) where T : class
        {
            if (!File.Exists(configurationFilePath))
            {
                this.EventAggregator.PublishOnUIThread(
                    new LogEntry(
                        "Configuration file " + configurationFilePath + "not found", 
                        LogEntrySeverity.Error, 
                        LogEntrySource.UI, 
                        Path.GetDirectoryName(configurationFilePath)));
                
                return null;
            }

            var config = XDocument.Load(configurationFilePath, LoadOptions.SetLineInfo);

            this.OnConfigLoaded(config);

            return this.BuildConfiguration<T>(config);
        }

        private ObservableCollection<T> BuildConfiguration<T>(XDocument config) where T : class
        {
            ObservableCollection<T> elements = new ObservableCollection<T>();

            var foundElements = config.Descendants(this.xmlElementIdentifier);

            foreach (var element in foundElements)//.Where(e => this.filter(e)))
            {
                elements.Add(this.OnBuildElement<T>(element));
            }

            return elements;
        }

        protected abstract T OnBuildElement<T>(XElement element) where T : class;

        protected virtual void OnConfigLoaded(XDocument config)
        {
        }
    }
}
