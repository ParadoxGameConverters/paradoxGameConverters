using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Globalization;

namespace Converter.UI.Helpers
{
    public static class NumberParserHelper
    {
        public static bool TryParseDouble(string s, out double result)
        {
            return double.TryParse(s, NumberStyles.Number, CultureInfo.InvariantCulture, out result);
        }
    }
}
