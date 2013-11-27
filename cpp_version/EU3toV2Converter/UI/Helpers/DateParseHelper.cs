using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;

namespace Converter.UI.Helpers
{
    public static class DateParseHelper
    {
        /// <summary>
        /// Tries the parse double.
        /// </summary>
        /// <param name="s">The arguments.</param>
        /// <param name="result">The result.</param>
        /// <returns></returns>
        public static bool TryParseDate(string s, string dateFormat, out DateTime result)
        {
            return DateTime.TryParseExact(s, dateFormat, CultureInfo.InvariantCulture, DateTimeStyles.None, out result);
        }

        /// <summary>
        /// Tries the parse double.
        /// </summary>
        /// <param name="s">The arguments.</param>
        /// <param name="result">The result.</param>
        /// <returns></returns>
        public static bool TryParseDate(string s, out DateTime result)
        {
            return DateTime.TryParse(s, CultureInfo.InvariantCulture, DateTimeStyles.None, out result);
        }
    }
}
