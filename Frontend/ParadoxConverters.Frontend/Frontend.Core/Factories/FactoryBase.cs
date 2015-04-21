using System;
using System.Collections.ObjectModel;
using System.IO;
using System.Xml.Linq;
using Caliburn.Micro;
using Frontend.Core.Helpers;
using Frontend.Core.Logging;

namespace Frontend.Core.Factories
{
    /// <summary>
    ///     Base class for all factories
    /// </summary>
    public abstract class FactoryBase
    {
        // The event aggregator

        // The tag in the xml file that is the root element for the type of object this factory should create
        private readonly string xmlElementIdentifier;

        /// <summary>
        ///     Initializes a new instance of the FactoryBase class
        /// </summary>
        /// <param name="eventAggregator">The event aggregator</param>
        /// <param name="xmlElementIdentifier">The xml tag to start with</param>
        /// <param name="filter">The filter (NOT IMPLEMENTED)</param>
        protected FactoryBase(IEventAggregator eventAggregator, string xmlElementIdentifier, Func<XElement, bool> filter)
        {
            EventAggregator = eventAggregator;
            this.xmlElementIdentifier = xmlElementIdentifier;
            //this.filter = filter;
        }

        /// <summary>
        ///     Initializes a new instance of the FactoryBase class
        /// </summary>
        /// <param name="eventAggregator">The eventAggregator</param>
        /// <param name="xmlElementIdentifier">The xml tag to start with</param>
        protected FactoryBase(IEventAggregator eventAggregator, string xmlElementIdentifier)
            : this(eventAggregator, xmlElementIdentifier, null)
        {
        }

        /// <summary>
        ///     Gets the Event Aggregator
        /// </summary>
        protected IEventAggregator EventAggregator { get; private set; }

        /// <summary>
        ///     Build model objects from the specified configuration file
        /// </summary>
        /// <typeparam name="T">The type of the model object</typeparam>
        /// <param name="configurationFilePath">The path to the configuratio file</param>
        /// <returns>
        ///     Null if the configuration file doesn't exist, the model object of type T if the model object was created
        ///     successfully
        /// </returns>
        public ObservableCollection<T> BuildModels<T>(string configurationFilePath) where T : class
        {
            if (!File.Exists(configurationFilePath))
            {
                EventAggregator.PublishOnUIThread(
                    new LogEntry(
                        "Configuration file " + configurationFilePath + "not found",
                        LogEntrySeverity.Error,
                        LogEntrySource.UI,
                        Path.GetDirectoryName(configurationFilePath)));

                return null;
            }

            var config = XDocument.Load(configurationFilePath, LoadOptions.SetLineInfo);

            OnConfigLoaded(config);

            return BuildConfiguration<T>(config);
        }

        /// <summary>
        ///     Essentially a filter method, this method excludes any disabled element in the xml configuration file
        /// </summary>
        /// <remarks>
        ///     For now, this only looks for the isEnabled=false tag. With time, a proper filter may be implemented.
        ///     Todo:
        ///     Simplify
        ///     Rename to something descriptive?
        /// </remarks>
        /// <typeparam name="T">The type of element</typeparam>
        /// <param name="config">The configuration file</param>
        /// <returns>The model objects</returns>
        public ObservableCollection<T> BuildConfiguration<T>(XDocument config) where T : class
        {
            var elements = new ObservableCollection<T>();

            var foundElements = config.Descendants(xmlElementIdentifier);

            foreach (var element in foundElements) //.Where(e => this.filter(e)))
            {
                var isEnabled = XElementHelper.ReadBoolValue(element, "isEnabled", false, true);

                if (isEnabled)
                {
                    elements.Add(OnBuildElement<T>(element));
                }
            }

            return elements;
        }

        /// <summary>
        ///     Abstract method responsible for building the model object.
        /// </summary>
        /// <typeparam name="T">The type of the model object</typeparam>
        /// <param name="element">The XElement (xml element) the model should be created from</param>
        /// <returns>The generated model object</returns>
        protected abstract T OnBuildElement<T>(XElement element) where T : class;

        /// <summary>
        ///     Essentially a hack needed to read things in the right order (?), this method is fired
        ///     after the configuration file is loaded from disk, but before the model object construction starts.
        /// </summary>
        /// <param name="config">The configuration file that was loaded</param>
        protected virtual void OnConfigLoaded(XDocument config)
        {
        }
    }
}