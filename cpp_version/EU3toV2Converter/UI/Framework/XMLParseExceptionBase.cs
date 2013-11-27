using System;
using System.Xml.Linq;

namespace Converter.UI.Framework
{
    /// <summary>
    /// Base class for the various XML parse exceptions
    /// </summary>
    public abstract class XMLParseExceptionBase : Exception
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="XMLParseExceptionBase"/> class.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <param name="isPropertyRequired">if set to <c>true</c> [is property required].</param>
        public XMLParseExceptionBase(XElement parentElement, string propertyName, bool isPropertyRequired)
        {
            this.ParentElement = parentElement;
            this.PropertyName = propertyName;
            this.IsPropertyRequired = isPropertyRequired;
        }

        /// <summary>
        /// Gets the parent element.
        /// </summary>
        /// <value>
        /// The parent element.
        /// </value>
        public XElement ParentElement { get; private set; }

        /// <summary>
        /// Gets the name of the property.
        /// </summary>
        /// <value>
        /// The name of the property.
        /// </value>
        public string PropertyName { get; private set; }

        /// <summary>
        /// Gets a value indicating whether [is property required].
        /// </summary>
        /// <value>
        ///   <c>true</c> if [is property required]; otherwise, <c>false</c>.
        /// </value>
        public bool IsPropertyRequired { get; private set; }
    }
}
