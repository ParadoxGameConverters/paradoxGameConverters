using Frontend.Core.Model.Preferences.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Model.Preferences
{
    public class NumericPreference : Preference<double>, INumericPreference
    {
        protected override string OnValidateProperty(string propertyName)
        {
            if (this.MinValue >= double.MinValue && this.MaxValue >= double.MinValue)
            {
                return this.ValidateNumericValues(propertyName);
            }

            return null;
        }

        /// <summary>
        /// Validating numeric values
        /// </summary>
        /// <param name="propertyName">The name of the property to validate</param>
        /// <returns>A string describing any problems with the current value</returns>
        private string ValidateNumericValues(string propertyName)
        {
            //double value;
            //bool canParse = NumberParserHelper.TryParseDouble(this.Value, out value);

            //if (!canParse)
            //{
            //    return "This value must be numeric. Please verify that you're using the correct delimiter such as \",\" or \".\"";
            //}

            //TODO: This currently accepts both "," and "." as delimeters for some culture settings. Which can lead to confusing number parsing

            if (this.Value < this.MinValue || this.Value > this.MaxValue)
            {
                return "The value must be within the limits specified by the minimum and maximum value, currently " + this.MinValue + " and " + this.MaxValue + " respectively.";
            }

            return null;
        }
    }
}
