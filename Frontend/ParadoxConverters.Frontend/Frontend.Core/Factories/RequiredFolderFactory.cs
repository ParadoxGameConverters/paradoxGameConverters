using System.Xml.Linq;
using Caliburn.Micro;
using Frontend.Core.Factories.TagReaders;

namespace Frontend.Core.Factories
{
    public class RequiredFolderFactory : FactoryBase
    {
        private readonly FolderTagReader folderTagReader;

        /// <summary>
        ///     Initializes a new instance of the RequiredItemFactory
        /// </summary>
        /// <param name="eventAggregator">The event aggregator</param>
        public RequiredFolderFactory(IEventAggregator eventAggregator)
            : base(eventAggregator, "requiredFolder")
        {
            folderTagReader = new FolderTagReader(eventAggregator);
        }

        protected override T OnBuildElement<T>(XElement element)
        {
            return (T) folderTagReader.ReadFolder(element);
        }
    }
}