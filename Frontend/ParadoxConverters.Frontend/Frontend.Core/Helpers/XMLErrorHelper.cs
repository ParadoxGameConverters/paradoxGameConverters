using System;
using System.Windows;
using System.Xml;
using System.Xml.Linq;
using Frontend.Core.Exceptions;

namespace Frontend.Core.Helpers
{
    public static class XMLErrorHelper
    {
        /// <summary>
        ///     Shows an (helpful?) error message if something goes wrong during parsing.
        /// </summary>
        /// <param name="xmlFileName">Name of the XML file.</param>
        /// <param name="exception">The exception.</param>
        public static void ShowMalformedOrMissingXMLElementError(string xmlFileName, XMLParseExceptionBase exception)
        {
            var lineInfo = FindExpectedLineNumber(exception.ParentElement);

            var message = "There was an error reading " + xmlFileName;

            if (lineInfo != null)
            {
                message += ". The element starting at " + lineInfo.Item1 + ", position: " + lineInfo.Item2 +
                           " seems to be missing subitem named " + exception.PropertyName + ".";
            }
            else
            {
                message += "; the element named " + exception.PropertyName + " couldn't be properly read.";
            }

            MessageBox.Show(message, "Configuration file error", MessageBoxButton.OK, MessageBoxImage.Error);
        }

        /// <summary>
        ///     Finds the expected line number. Useful for the user if something went wrong during parsing
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <returns></returns>
        private static Tuple<int, int> FindExpectedLineNumber(XElement parentElement)
        {
            IXmlLineInfo info = parentElement;

            return info.HasLineInfo() ? new Tuple<int, int>(info.LineNumber, info.LinePosition) : null;
        }
    }
}