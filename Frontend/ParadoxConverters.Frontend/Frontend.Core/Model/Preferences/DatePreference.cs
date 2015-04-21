using System;
using System.Text;
using Frontend.Core.Model.Preferences.Interfaces;

namespace Frontend.Core.Model.Preferences
{
    public class DatePreference : Preference<DateTime>, IDatePreference
    {
        /// <summary>
        ///     Gets or sets the date format
        /// </summary>
        public string DateFormat { get; set; }

        /// <summary>
        ///     Validates properties
        /// </summary>
        /// <param name="propertyName">The name of the property that changed</param>
        /// <returns>Whether or not there are any problems with the value in this property</returns>
        protected override string OnValidateProperty(string propertyName)
        {
            if (MinValue > DateTime.MinValue && MaxValue > DateTime.MinValue)
            {
                return ValidateDates(propertyName);
            }

            return null;
        }

        /// <summary>
        ///     Validating dates
        /// </summary>
        /// <param name="propertyName">The name of the property to validate</param>
        /// <returns>A string describing the problem with the current value</returns>
        private string ValidateDates(string propertyName)
        {
            if (Value < MinValue || Value > MaxValue)
            {
                return "The value must be within the limits specified by the minimum and maximum value, currently " +
                       MinValue + " and " + MaxValue + " respectively.";
            }

            return null;
        }

        /// <summary>
        ///     Override tostring to strip out time component of date.
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            if (Value != null)
            {
                // Reasoning: The converter does not seem to care about the time component of the date even when present, but it's safer to remove it.

                var sb = new StringBuilder();

                sb.AppendLine("# " + FriendlyName + ": " + Description);

                AppendEntries(sb);

                sb.AppendLine("\t" + Name + " = \"" + Value.ToString("d") + "\"");

                return sb.ToString();
            }

            return base.ToString();
        }
    }
}