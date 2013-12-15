using System.Xml.Linq;

namespace Converter.UI.Framework
{
    /// <summary>
    /// Exception thrown when the xml that's being parsed is malformed
    /// </summary>
    public class MalformedXMLException : XMLParseExceptionBase
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="MalformedXMLException"/> class.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <param name="isPropertyRequired">if set to <c>true</c> [is property required].</param>
        public MalformedXMLException(XElement parentElement, string propertyName, bool isPropertyRequired)
            : base(parentElement, propertyName, isPropertyRequired)
        {
        }
    }
}
