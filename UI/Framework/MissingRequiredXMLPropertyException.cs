using System.Xml.Linq;

namespace Converter.UI.Framework
{
    public class MissingRequiredXMLPropertyException : XMLParseExceptionBase
    {
        public MissingRequiredXMLPropertyException(XElement parentElement, string propertyName, bool isPropertyRequired)
            : base(parentElement, propertyName, isPropertyRequired)
        {
        }
    }
}
