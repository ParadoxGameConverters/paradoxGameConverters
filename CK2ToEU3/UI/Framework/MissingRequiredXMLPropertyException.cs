using System.Xml.Linq;

namespace Converter.UI.Framework
{
    /// <summary>
    /// Exception that will be thrown if the xml that is being parsed is missing some required property
    /// </summary>
    public class MissingRequiredXMLPropertyException : XMLParseExceptionBase
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="MissingRequiredXMLPropertyException"/> class.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <param name="isPropertyRequired">if set to <c>true</c> [is property required].</param>
        public MissingRequiredXMLPropertyException(XElement parentElement, string propertyName, bool isPropertyRequired)
            : base(parentElement, propertyName, isPropertyRequired)
        {
        }
    }
}
