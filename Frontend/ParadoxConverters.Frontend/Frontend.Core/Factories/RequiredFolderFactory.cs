using Caliburn.Micro;
using Frontend.Core.Factories.TagReaders;
using Frontend.Core.Helpers;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace Frontend.Core.Factories
{
    public class RequiredFolderFactory : FactoryBase
    {
        private FolderTagReader folderTagReader;

        /// <summary>
        /// Initializes a new instance of the RequiredItemFactory
        /// </summary>
        /// <param name="eventAggregator">The event aggregator</param>
        public RequiredFolderFactory(IEventAggregator eventAggregator)
            : base(eventAggregator, "requiredFolder")
        {
            this.folderTagReader = new FolderTagReader(eventAggregator);
        }

        protected override T OnBuildElement<T>(XElement element)
        {
            return (T)this.folderTagReader.ReadFolder(element);
        }
    }
}
