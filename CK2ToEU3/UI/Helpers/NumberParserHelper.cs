using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Globalization;

namespace Converter.UI.Helpers
{
    /// <summary>
    /// Helper class for various parse operations on numbers
    /// </summary>
    public static class NumberParserHelper
    {
        /// <summary>
        /// Tries the parse double.
        /// </summary>
        /// <param name="s">The arguments.</param>
        /// <param name="result">The result.</param>
        /// <returns></returns>
        public static bool TryParseDouble(string s, out double result)
        {
            return double.TryParse(s, NumberStyles.Number, CultureInfo.InvariantCulture, out result);
        }
    }
}
