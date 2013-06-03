using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Xml.Linq;
using Converter.UI.Framework;

namespace Converter.UI.Helpers
{
    public static class XElementHelper
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="parentElement"></param>
        /// <param name="propertyName"></param>
        /// <returns></returns>
        public static double ReadDoubleValue(XElement parentElement, string propertyName)
        {
            return ReadDoubleValue(parentElement, propertyName, true);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="parentElement"></param>
        /// <param name="propertyName"></param>
        /// <param name="isPropertyRequired"></param>
        /// <returns></returns>
        public static double ReadDoubleValue(XElement parentElement, string propertyName, bool isPropertyRequired)
        {
            double value;
            IEnumerable<XElement> matches = parentElement.Descendants(propertyName);
            XElement match;
            bool foundMatch = false;

            if (matches.Count() > 0)
            {
                match = matches.First();
                foundMatch = double.TryParse(match.Value, NumberStyles.Number, new CultureInfo("en-US"), out value);

                return value;
            }
            else if (!foundMatch && isPropertyRequired)
            {
                throw new MissingRequiredXMLPropertyException(parentElement, propertyName, isPropertyRequired);
            }
            else
            {
                throw new MissingRequiredXMLPropertyException(parentElement, propertyName, isPropertyRequired);
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="parentElement"></param>
        /// <param name="propertyName"></param>
        /// <returns></returns>
        public static string ReadStringValue(XElement parentElement, string propertyName)
        {
            return ReadStringValue(parentElement, propertyName, true);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="parentElement"></param>
        /// <param name="propertyName"></param>
        /// <param name="isPropertyRequired"></param>
        /// <returns></returns>
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
        /// 
        /// </summary>
        /// <param name="parentElement"></param>
        /// <param name="propertyName"></param>
        /// <returns></returns>
        public static bool ReadBoolValue(XElement parentElement, string propertyName)
        {
            return ReadBoolValue(parentElement, propertyName, true);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="parentElement"></param>
        /// <param name="propertyName"></param>
        /// <param name="isPropertyRequired"></param>
        /// <returns></returns>
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
        /// 
        /// </summary>
        /// <param name="parentElement"></param>
        /// <param name="propertyName"></param>
        /// <returns></returns>
        public static IEnumerable<XElement> ReadEnumerable(XElement parentElement, string propertyName)
        {
            return ReadEnumerable(parentElement, propertyName, true);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="parentElement"></param>
        /// <param name="propertyName"></param>
        /// <param name="isPropertyRequired"></param>
        /// <returns></returns>
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
