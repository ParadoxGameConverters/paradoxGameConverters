using System.Xml.Linq;

namespace Converter.UI.Framework
{
    /// <summary>
    /// Exception thrown when a data value in the xml file that's being parsed is, well, unparsable.
    /// </summary>
    public class UnParsableDataValueException : XMLParseExceptionBase
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="UnParsableDataValueException"/> class.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <param name="isPropertyRequired">if set to <c>true</c> [is property required].</param>
        /// <param name="expectedType">The expected type.</param>
        public UnParsableDataValueException(XElement parentElement, string propertyName, bool isPropertyRequired, string expectedType)
            : base(parentElement, propertyName, isPropertyRequired)
        {
            this.ExpectedType = expectedType;
        }

        /// <summary>
        /// Gets the expected type.
        /// </summary>
        /// <value>
        /// The expected type.
        /// </value>
        public string ExpectedType { get; private set; }
    }
}
