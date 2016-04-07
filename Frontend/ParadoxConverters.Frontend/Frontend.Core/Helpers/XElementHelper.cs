using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using Frontend.Core.Exceptions;

namespace Frontend.Core.Helpers
{
    /// <summary>
    ///     Helper class for various XElement reading tasks
    /// </summary>
    public static class XElementHelper
    {
        /// <summary>
        ///     Reads the double value.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <returns></returns>
        public static double ReadDoubleValue(XElement parentElement, string propertyName)
        {
            return ReadDoubleValue(parentElement, propertyName, true);
        }

        /// <summary>
        ///     Reads the double value.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <param name="isPropertyRequired">if set to <c>true</c> [is property required].</param>
        /// <returns></returns>
        /// <exception cref="Converter.UI.Framework.MissingRequiredXMLPropertyException"></exception>
        public static double ReadDoubleValue(XElement parentElement, string propertyName, bool isPropertyRequired)
        {
            double value;
            var matches = parentElement.Descendants(propertyName);
            XElement match;
            var foundMatch = false;

            if (matches.Count() > 0)
            {
                match = matches.First();
                foundMatch = NumberParserHelper.TryParseDouble(match.Value, out value);

                return value;
            }
            if (!foundMatch && isPropertyRequired)
            {
                throw new MissingRequiredXMLPropertyException(parentElement, propertyName, isPropertyRequired);
            }
            return 0d;
        }

        /// <summary>
        ///     Reads the datetime value.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <returns></returns>
        public static DateTime ReadDateValue(XElement parentElement, string propertyName, string dateFormat)
        {
            return ReadDateValue(parentElement, propertyName, dateFormat, true);
        }

        /// <summary>
        ///     Reads the datetime value.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <param name="isPropertyRequired">if set to <c>true</c> [is property required].</param>
        /// <returns></returns>
        /// <exception cref="Converter.UI.Framework.MissingRequiredXMLPropertyException"></exception>
        public static DateTime ReadDateValue(XElement parentElement, string propertyName, string dateFormat,
            bool isPropertyRequired)
        {
            DateTime value;
            var matches = parentElement.Descendants(propertyName);
            XElement match;
            var foundMatch = false;

            if (matches.Count() > 0)
            {
                match = matches.First();
                foundMatch = DateParseHelper.TryParseDate(match.Value, dateFormat, out value);

                // Strip out the time component. As explained in the following url, TryParseExact and TryParse can return different results for the same input:
                // http://social.msdn.microsoft.com/Forums/vstudio/en-US/94a1eaed-1ec3-4eed-aa6e-a8f44a63a0e8/date-validation-tryparse-and-tryparseexact-didnt-get-the-same-result
                // Stripping out the time component should negate this difference
                return value.Date;
            }
            if (!foundMatch && isPropertyRequired)
            {
                throw new MissingRequiredXMLPropertyException(parentElement, propertyName, isPropertyRequired);
            }
            return DateTime.Now;
        }

        /// <summary>
        ///     Reads the string value.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <returns></returns>
        public static string ReadStringValue(XElement parentElement, string propertyName)
        {
            return ReadStringValue(parentElement, propertyName, true);
        }

        /// <summary>
        ///     Reads the string value.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <param name="isPropertyRequired">if set to <c>true</c> [is property required].</param>
        /// <returns></returns>
        /// <exception cref="Converter.UI.Framework.MissingRequiredXMLPropertyException">
        /// </exception>
        public static string ReadStringValue(XElement parentElement, string propertyName, bool isPropertyRequired)
        {
            string value;

            var matches = parentElement.Descendants(propertyName);

            if (matches.Count() == 0)
            {
                if (isPropertyRequired)
                {
                    throw new MissingRequiredXMLPropertyException(parentElement, propertyName, isPropertyRequired);
                }
                return string.Empty;
            }

            var match = matches.First();

            value = match.Value;

            if (string.IsNullOrEmpty(value) && isPropertyRequired)
            {
                throw new MissingRequiredXMLPropertyException(parentElement, propertyName, isPropertyRequired);
            }

            return value;
        }

        /// <summary>
        ///     Reads the bool value.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <returns></returns>
        public static bool ReadBoolValue(XElement parentElement, string propertyName)
        {
            return ReadBoolValue(parentElement, propertyName, true);
        }

        /// <summary>
        ///     Reads the bool value.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <param name="isPropertyRequired">if set to <c>true</c> [is property required].</param>
        /// <returns></returns>
        /// <exception cref="MissingRequiredXMLPropertyException"></exception>
        /// <exception cref="UnparsableDataValueException">bool</exception>
        public static bool ReadBoolValue(XElement parentElement, string propertyName, bool isPropertyRequired)
        {
            return ReadBoolValue(parentElement, propertyName, isPropertyRequired, false);
        }

        public static bool ReadBoolValue(XElement parentElement, string propertyName, bool isPropertyRequired,
            bool defaultValue)
        {
            var value = defaultValue;
            var matches = parentElement.Descendants(propertyName);

            if (!matches.Any())
            {
                if (isPropertyRequired)
                {
                    throw new MissingRequiredXMLPropertyException(parentElement, propertyName, isPropertyRequired);
                }
                return defaultValue;
            }

            var match = matches.First();

            var foundMatch = bool.TryParse(match.Value, out value);

            if (!foundMatch && isPropertyRequired)
            {
                throw new UnparsableDataValueException(parentElement, propertyName, isPropertyRequired, "bool");
            }

            return value;
        }

        /// <summary>
        ///     Reads the enumerable.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <returns></returns>
        public static IEnumerable<XElement> ReadEnumerable(XElement parentElement, string propertyName)
        {
            return ReadEnumerable(parentElement, propertyName, true);
        }

        /// <summary>
        ///     Reads the enumerable.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <param name="isPropertyRequired">if set to <c>true</c> [is property required].</param>
        /// <returns></returns>
        /// <exception cref="Converter.UI.Framework.MissingRequiredXMLPropertyException"></exception>
        public static IEnumerable<XElement> ReadEnumerable(XElement parentElement, string propertyName,
            bool isPropertyRequired)
        {
            var elements = parentElement.Descendants(propertyName);

            if (!elements.Any() && isPropertyRequired)
            {
                throw new MissingRequiredXMLPropertyException(parentElement, propertyName, isPropertyRequired);
            }

            return elements;
        }
    }
}