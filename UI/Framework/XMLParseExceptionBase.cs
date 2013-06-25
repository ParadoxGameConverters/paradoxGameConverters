using System;
using System.Xml.Linq;

namespace Converter.UI.Framework
{
    public abstract class XMLParseExceptionBase : Exception
    {
        public XMLParseExceptionBase(XElement parentElement, string propertyName, bool isPropertyRequired)
        {
            this.ParentElement = parentElement;
            this.PropertyName = propertyName;
            this.IsPropertyRequired = isPropertyRequired;
        }

        public XElement ParentElement { get; private set; }
        public string PropertyName { get; private set; }
        public bool IsPropertyRequired { get; private set; }
    }
}
