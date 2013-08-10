using System.Xml.Linq;

namespace Converter.UI.Framework
{
    public class MalformedXMLException : XMLParseExceptionBase
    {
        public MalformedXMLException(XElement parentElement, string propertyName, bool isPropertyRequired)
            : base(parentElement, propertyName, isPropertyRequired)
        {
        }
    }
}
