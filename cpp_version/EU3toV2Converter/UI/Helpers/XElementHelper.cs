using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Xml.Linq;
using Converter.UI.Framework;
using System;

namespace Converter.UI.Helpers
{
    /// <summary>
    /// Helper class for various XElement reading tasks
    /// </summary>
    public static class XElementHelper
    {
        /// <summary>
        /// Reads the double value.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <returns></returns>
        public static double ReadDoubleValue(XElement parentElement, string propertyName)
        {
            return ReadDoubleValue(parentElement, propertyName, true);
        }

        /// <summary>
        /// Reads the double value.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <param name="isPropertyRequired">if set to <c>true</c> [is property required].</param>
        /// <returns></returns>
        /// <exception cref="Converter.UI.Framework.MissingRequiredXMLPropertyException"></exception>
        public static double ReadDoubleValue(XElement parentElement, string propertyName, bool isPropertyRequired)
        {
            double value;
            IEnumerable<XElement> matches = parentElement.Descendants(propertyName);
            XElement match;
            bool foundMatch = false;

            if (matches.Count() > 0)
            {
                match = matches.First();
                foundMatch = NumberParserHelper.TryParseDouble(match.Value, out value);

                return value;
            }
            else if (!foundMatch && isPropertyRequired)
            {
                throw new MissingRequiredXMLPropertyException(parentElement, propertyName, isPropertyRequired);
            }
            else
            {
                return 0d;
            }
        }
        /// <summary>
        /// Reads the datetime value.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <returns></returns>
        public static DateTime ReadDateValue(XElement parentElement, string propertyName, string dateFormat)
        {
            return ReadDateValue(parentElement, propertyName, dateFormat, true);
        }

        /// <summary>
        /// Reads the datetime value.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <param name="isPropertyRequired">if set to <c>true</c> [is property required].</param>
        /// <returns></returns>
        /// <exception cref="Converter.UI.Framework.MissingRequiredXMLPropertyException"></exception>
        public static DateTime ReadDateValue(XElement parentElement, string propertyName, string dateFormat, bool isPropertyRequired)
        {
            DateTime value;
            IEnumerable<XElement> matches = parentElement.Descendants(propertyName);
            XElement match;
            bool foundMatch = false;

            if (matches.Count() > 0)
            {
                match = matches.First();
                foundMatch = DateParseHelper.TryParseDate(match.Value, dateFormat, out value);

                // Strip out the time component. As explained in the following url, TryParseExact and TryParse can return different results for the same input:
                // http://social.msdn.microsoft.com/Forums/vstudio/en-US/94a1eaed-1ec3-4eed-aa6e-a8f44a63a0e8/date-validation-tryparse-and-tryparseexact-didnt-get-the-same-result
                // Stripping out the time component should negate this difference
                return value.Date;
            }
            else if (!foundMatch && isPropertyRequired)
            {
                throw new MissingRequiredXMLPropertyException(parentElement, propertyName, isPropertyRequired);
            }
            else
            {
                return DateTime.Now;
            }
        }

        /// <summary>
        /// Reads the string value.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <returns></returns>
        public static string ReadStringValue(XElement parentElement, string propertyName)
        {
            return ReadStringValue(parentElement, propertyName, true);
        }

        /// <summary>
        /// Reads the string value.
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

            IEnumerable<XElement> matches = parentElement.Descendants(propertyName);

            if (matches.Count() == 0)
            {
                if (isPropertyRequired)
                {
                    throw new MissingRequiredXMLPropertyException(parentElement, propertyName, isPropertyRequired);
                }
                else
                {
                    return string.Empty;
                }
            }

            XElement match = matches.First();

            value = match.Value;

            if (string.IsNullOrEmpty(value) && isPropertyRequired)
            {
                throw new MissingRequiredXMLPropertyException(parentElement, propertyName, isPropertyRequired);
            }

            return value;
        }

        /// <summary>
        /// Reads the bool value.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <returns></returns>
        public static bool ReadBoolValue(XElement parentElement, string propertyName)
        {
            return ReadBoolValue(parentElement, propertyName, true);
        }

        /// <summary>
        /// Reads the bool value.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <param name="isPropertyRequired">if set to <c>true</c> [is property required].</param>
        /// <returns></returns>
        /// <exception cref="Converter.UI.Framework.MissingRequiredXMLPropertyException"></exception>
        /// <exception cref="Converter.UI.Framework.UnParsableDataValueException">bool</exception>
        public static bool ReadBoolValue(XElement parentElement, string propertyName, bool isPropertyRequired)
        {
            bool value = false;
            IEnumerable<XElement> matches = parentElement.Descendants(propertyName);

            if (matches.Count() == 0)
            {
                if (isPropertyRequired)
                {
                    throw new MissingRequiredXMLPropertyException(parentElement, propertyName, isPropertyRequired);
                }
                else
                {
                    return false;
                }
            }

            XElement match = matches.First();

            bool foundMatch = bool.TryParse(match.Value, out value);

            if (!foundMatch && isPropertyRequired)
            {
                throw new UnParsableDataValueException(parentElement, propertyName, isPropertyRequired, "bool");
            }

            return value;
        }

        /// <summary>
        /// Reads the enumerable.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <returns></returns>
        public static IEnumerable<XElement> ReadEnumerable(XElement parentElement, string propertyName)
        {
            return ReadEnumerable(parentElement, propertyName, true);
        }

        /// <summary>
        /// Reads the enumerable.
        /// </summary>
        /// <param name="parentElement">The parent element.</param>
        /// <param name="propertyName">Name of the property.</param>
        /// <param name="isPropertyRequired">if set to <c>true</c> [is property required].</param>
        /// <returns></returns>
        /// <exception cref="Converter.UI.Framework.MissingRequiredXMLPropertyException"></exception>
        public static IEnumerable<XElement> ReadEnumerable(XElement parentElement, string propertyName, bool isPropertyRequired)
        {
            IEnumerable<XElement> elements = parentElement.Descendants(propertyName);

            if (elements.Count() == 0 && isPropertyRequired)
            {
                throw new MissingRequiredXMLPropertyException(parentElement, propertyName, isPropertyRequired);
            }

            return elements;
        }

        ////private T ReadProperty<T>(XElement parentElement, string propertyName)
        ////{
        ////    T value = default(T);

        ////    if (typeof(T).GetInterfaces().Any(t => t.GetGenericTypeDefinition() == typeof(IEnumerable)))
        ////    {
        ////        value = (T)parentElement.Descendants(propertyName);
        ////    }
        ////    else
        ////    {
        ////        var xmlElement = parentElement.Descendants(propertyName);

        ////        if (xmlElement.Count() > 0)
        ////        {
        ////            if (typeof(T).Equals(typeof(string)))
        ////            {
        ////                var output = TryParse<string>(xmlElement.First().Value, );
        ////            }
        ////            else if(typeof(T).Equals(typeof(int)))
        ////            {
        ////            }
        ////        }
        ////    }

        ////    return value;
        ////}

        ////public static T? TryParse<T>(string value, TryParseHandler<T> handler) where T : struct
        ////{
        ////    if (String.IsNullOrEmpty(value))
        ////        return null;
        ////    T result;
        ////    if (handler(value, out result))
        ////        return result;
        ////    Trace.TraceWarning("Invalid value '{0}'", value);
        ////    return null;
        ////}

        ////public delegate bool TryParseHandler<T>(string value, out T result);
    }
}
