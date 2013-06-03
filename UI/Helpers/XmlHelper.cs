using System.IO;
using System.Xml.Linq;

namespace Converter.UI.Helpers
{
    public static class XmlHelper
    {
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
