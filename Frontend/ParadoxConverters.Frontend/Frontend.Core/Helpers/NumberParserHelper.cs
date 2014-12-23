using System.Globalization;

namespace Frontend.Core.Helpers
{
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
