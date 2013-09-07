using System.IO;
using System.Xml.Linq;

namespace Converter.UI.Helpers
{
    /// <summary>
    /// Helper class for various XML related read operations
    /// </summary>
    public static class XmlHelper
    {
        /// <summary>
        /// Reads the XML file.
        /// </summary>
        /// <param name="filePath">The file path.</param>
        /// <returns></returns>
        public static XDocument ReadXmlFile(string filePath)
        {
            if (!File.Exists(filePath))
            {
                return null;
            }

            return XDocument.Load(filePath, LoadOptions.SetLineInfo);
        }
    }
}
