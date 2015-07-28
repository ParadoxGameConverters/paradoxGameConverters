using System.Collections.Generic;
using System.IO;
using System.Xml.Linq;
using Caliburn.Micro;
using Frontend.Core.Common.Proxies;
using Frontend.Core.Helpers;
using Frontend.Core.Model.Paths;
using Frontend.Core.Model.Paths.Interfaces;

namespace Frontend.Core.Factories.TagReaders
{
    public class FileTagReader : TagReaderBase
    {
        public FileTagReader(IEventAggregator eventAggregator)
            : base(eventAggregator, new DirectoryHelper(), new EnvironmentProxy())
        {
        }

        public IRequiredFile ReadFile(XElement xmlElement)
        {
            var tagName = XElementHelper.ReadStringValue(xmlElement, "tag", false);
            var internalTagName = XElementHelper.ReadStringValue(xmlElement, "internalTag", false);
            var friendlyName = XElementHelper.ReadStringValue(xmlElement, "friendlyName");
            var description = XElementHelper.ReadStringValue(xmlElement, "description");
            var extension = XElementHelper.ReadStringValue(xmlElement, "extension", false);
            var predefinedFileName = XElementHelper.ReadStringValue(xmlElement, "predefinedFileName", false);
            var alternativePaths = ReadDefaultLocationPaths(xmlElement, tagName, friendlyName, predefinedFileName);
            var isMandatory = XElementHelper.ReadBoolValue(xmlElement, "isMandatory", false);
            var hidden = XElementHelper.ReadBoolValue(xmlElement, "hidden", false);
           
            return BuildRequiredFolderObject(tagName, alternativePaths, friendlyName, description, extension,
                predefinedFileName, internalTagName, isMandatory, hidden);
        }

        /// <summary>
        ///     Todo: Move this to a separate factory somehow.
        /// </summary>
        /// <param name="tagName"></param>
        /// <param name="defaultValue"></param>
        /// <returns></returns>
        private IRequiredFile BuildRequiredFolderObject(string tagName, IList<IAlternativePath> alternatives,
            string friendlyName, string description, string extension, string predefinedFileName, string internalTagName,
            bool isMandatory, bool hidden)
        {
            return new RequiredFile(tagName, friendlyName, description, alternatives, extension, predefinedFileName,
                internalTagName, isMandatory, hidden);
        }
    }
}