using System.Xml.Linq;
using Caliburn.Micro;
using Frontend.Core.Factories.TagReaders;

namespace Frontend.Core.Factories
{
    public class RequiredFileFactory : FactoryBase
    {
        private readonly FileTagReader fileTagReader;

        /// <summary>
        ///     Initializes a new instance of the RequiredFileFactory
        /// </summary>
        /// <param name="eventAggregator">The event aggregator</param>
        public RequiredFileFactory(IEventAggregator eventAggregator)
            : base(eventAggregator, "requiredFile")
        {
            fileTagReader = new FileTagReader(eventAggregator);
        }

        protected override T OnBuildElement<T>(XElement element)
        {
            return (T) fileTagReader.ReadFile(element);
        }
    }
}