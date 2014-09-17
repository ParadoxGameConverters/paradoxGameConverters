using Caliburn.Micro;
using Frontend.Core.Helpers;
using Frontend.Core.Model;
using Frontend.Core.Model.Paths;
using Frontend.Core.Model.Paths.Interfaces;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace Frontend.Core.Factories.TagReaders
{
    public class FileTagReader : TagReaderBase
    {
        public FileTagReader(IEventAggregator eventAggregator)
            : base(eventAggregator)
        {
        }

        public IRequiredFile ReadFile(XElement xmlElement)
        {
            var tagName = XElementHelper.ReadStringValue(xmlElement, "tag", false);
            var friendlyName = XElementHelper.ReadStringValue(xmlElement, "friendlyName");
            var description = XElementHelper.ReadStringValue(xmlElement, "description");
            var extension = XElementHelper.ReadStringValue(xmlElement, "extension", false);
            var predefinedFileName = XElementHelper.ReadStringValue(xmlElement, "predefinedFileName", false);
            var alternativePaths = this.ReadDefaultLocationPaths(xmlElement);

            // If a filename is set, add it to all the alternative paths.
            // This basically turns a folder path into a file path.
            if (!string.IsNullOrEmpty(predefinedFileName))
            {
                foreach (var alternative in alternativePaths)
                {
                    alternative.Path = Path.Combine(alternative.Path, predefinedFileName);
                }
            }

            return this.BuildRequiredFolderObject(tagName, alternativePaths, friendlyName, description, extension, predefinedFileName);
        }

        /// <summary>
        /// Todo: Move this to a separate factory somehow.
        /// </summary>
        /// <param name="tagName"></param>
        /// <param name="defaultValue"></param>
        /// <returns></returns>
        private IRequiredFile BuildRequiredFolderObject(string tagName, IList<IAlternativePath> alternatives, string friendlyName, string description, string extension, string predefinedFileName)
        {
            return new RequiredFile(tagName, friendlyName, description, alternatives, extension, predefinedFileName);
        }
    }
}
