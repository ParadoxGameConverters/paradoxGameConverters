using System.Xml.Linq;

namespace Converter.UI.Framework
{
    public class UnParsableDataValueException : XMLParseExceptionBase
    {
        public UnParsableDataValueException(XElement parentElement, string propertyName, bool isPropertyRequired, string expectedType)
            : base(parentElement, propertyName, isPropertyRequired)
        {
            this.ExpectedType = expectedType;
        }

        public string ExpectedType { get; private set; }
    }
}
