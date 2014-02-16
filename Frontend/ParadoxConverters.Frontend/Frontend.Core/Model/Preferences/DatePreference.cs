using Frontend.Core.Model.Preferences.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Model.Preferences
{
    public class DatePreference : Preference<DateTime>, IDatePreference
    {
        /// <summary>
        /// Gets or sets the date format
        /// </summary>
        public string DateFormat { get; set; }

        /// <summary>
        /// Validates properties
        /// </summary>
        /// <param name="propertyName">The name of the property that changed</param>
        /// <returns>Whether or not there are any problems with the value in this property</returns>
        protected override string OnValidateProperty(string propertyName)
        {
            if (this.MinValue > DateTime.MinValue && this.MaxValue > DateTime.MinValue)
            {
                return this.ValidateDates(propertyName);
            }

            return null;
        }

        /// <summary>
        /// Validating dates
        /// </summary>
        /// <param name="propertyName">The name of the property to validate</param>
        /// <returns>A string describing the problem with the current value</returns>
        private string ValidateDates(string propertyName)
        {
            if (this.Value < this.MinValue || this.Value > this.MaxValue)
            {
                return "The value must be within the limits specified by the minimum and maximum value, currently " + this.MinValue + " and " + this.MaxValue + " respectively.";
            }

            return null;
        }
    }
}
