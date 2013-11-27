using System.Collections.Generic;
using Converter.UI.Framework;
using Converter.UI.Enums;
using System.Text;
using System.Linq;
using System;
using System.ComponentModel;
using System.Globalization;
using System.Windows;
using Converter.UI.Helpers;

namespace Converter.UI.Settings
{
    /// <summary>
    /// Implementation of IPreference. 
    /// <remarks>Also implements IDataErrorInfo, which is used for validating some user input in user interface, such as minimum and maximum values.</remarks>
    /// </summary>
    public class Preference : NotifiableBase, IPreference, IDataErrorInfo
    {
        private IList<IPreferenceEntry> entries;
        private IPreferenceEntry selectedEntry;
        private string value;

        /// <summary>
        /// Gets or sets the preference name. Must match the name of the preference in configuration.txt
        /// </summary>
        /// <value>
        /// The name.
        /// </value>
        public string Name { get; set; }

        /// <summary>
        /// Gets or sets the friendly preference name. Usually more readable to humans than the normal name, which tends to lack - for instance - spaces.
        /// </summary>
        /// <value>
        /// The name of the friendly.
        /// </value>
        public string FriendlyName { get; set; }

        /// <summary>
        /// Gets or sets the description.
        /// </summary>
        /// <value>
        /// The description.
        /// </value>
        public string Description { get; set; }

        /// <summary>
        /// Gets or sets the minimum value.
        /// </summary>
        /// <value>
        /// The minimum value.
        /// </value>
        public double MinValue { get; set; }

        /// <summary>
        /// Gets or sets the maximum value.
        /// </summary>
        /// <value>
        /// The maximum value.
        /// </value>
        public double MaxValue { get; set; }
        
        /// <summary>
        /// Gets the list of pre-defined IPreferenceEntry objects. These are the pre-defined user choices, if not null.
        /// </summary>
        /// <value>
        /// The entries.
        /// </value>
        public IList<IPreferenceEntry> Entries
        {
            get
            {
                return this.entries ?? (this.entries = new List<IPreferenceEntry>());
            }
        }

        /// <summary>
        /// Gets or sets the selected entry. Only relevant if this list has a list of Entries to choose from.
        /// </summary>
        /// <value>
        /// The selected entry.
        /// </value>
        public IPreferenceEntry SelectedEntry
        {
            get
            {
                return this.selectedEntry;
            }

            set
            {
                if (this.selectedEntry == value)
                {
                    return;
                }

                this.selectedEntry = value;
                this.Value = value.Name;
                this.RaisePropertyChanged("SelectedEntry");
            }
        }

        /// <summary>
        /// Gets or sets the current value.
        /// </summary>
        /// <value>
        /// The value.
        /// </value>
        public string Value
        {
            get
            {
                return this.value;
            }

            set
            {
                if (this.value == value)
                {
                    return;
                }

                this.value = value;
                this.RaisePropertyChanged("Value");
            }
        }

        /// <summary>
        /// Gets a value indicating whether this preference has a set of predefined choices.
        /// </summary>
        /// <value>
        ///   <c>true</c> if yes; otherwise, <c>false</c>.
        /// </value>
        public bool HasPreDefinedChoices
        {
            get
            {
                return this.Entries.Count > 0;
            }
        }

        /// <summary>
        /// Gets or sets a value indicating whether this preference has a directly editable value (as opposed to pre-defined choices, for instance)
        /// </summary>
        /// <value>
        ///   <c>true</c> if yes; otherwise, <c>false</c>.
        /// </value>
        public bool HasDirectlyEditableValue { get; set; }

        /// <summary>
        /// Gets a value indicating whether this preference contains a numeric value
        /// </summary>
        /// <value>
        ///   <c>true</c> if yes, otherwise, <c>false</c>.
        /// </value>
        public bool IsNumeric
        {
            get
            {
                double value;
                return NumberParserHelper.TryParseDouble(this.Value, out value);
            }
        }

        /// <summary>
        /// Gets a value indicating whether this preference contains a date value
        /// </summary>
        public bool IsDate
        {
            get
            {
                DateTime value;
                return DateTime.TryParse(this.Value, out value);
            }
        }

        /// <summary>
        /// Gets or sets the minimum date value.
        /// </summary>
        /// <value>
        /// The minimum date value.
        /// </value>
        public DateTime MinDateValue { get; set; }

        /// <summary>
        /// Gets or sets the maximum date value.
        /// </summary>
        /// <value>
        /// The maximum date value.
        /// </value>
        public DateTime MaxDateValue { get; set; }

        #region [ Validation ]


        /// <summary>
        /// Gets an error message indicating what is wrong with this object. 
        /// </summary>
        /// <returns>An error message indicating what is wrong with this object. The default is an empty string ("").</returns>
        public string Error
        {
            get
            {
                return null;
            }
        }

        /// <summary>
        /// Gets the <see cref="System.String"/> with the specified name. Used by IDataErrorInfo
        /// </summary>
        /// <value>
        /// The <see cref="System.String"/>.
        /// </value>
        /// <param name="name">The name.</param>
        /// <returns></returns>
        public string this[string name]
        {
            get
            {
                string result = this.OnValidateProperty(name);

                return result;
            }
        }

        /// <summary>
        /// Gets a value indicating whether this preference has minimum and maximum values.
        /// </summary>
        /// <value>
        /// <c>true</c> if [has minimum maximum values]; otherwise, <c>false</c>.
        /// </value>
        private bool HasMinMaxValues
        {
            get
            {
                return this.MinValue >= double.MinValue && this.MaxValue >= double.MinValue;
            }
        }

        /// <summary>
        /// Gets a value indicating whether this preference has minimum and maximum date values.
        /// </summary>
        /// <value>
        /// <c>true</c> if [has minimum maximum date values]; otherwise, <c>false</c>.
        /// </value>
        private bool HasMinMaxDateValues
        {
            get
            {
                return this.MinDateValue > DateTime.MinValue && this.MaxDateValue > DateTime.MinValue;
            }
        }

        /// <summary>
        /// Called when [validate property].
        /// </summary>
        /// <param name="propertyName">Name of the property.</param>
        /// <returns></returns>
        protected virtual string OnValidateProperty(string propertyName)
        {
            if (this.HasMinMaxDateValues)
            {
                return this.ValidateDates(propertyName);
            }
            else if (this.HasMinMaxValues)
            {
                return this.ValidateNumericValues(propertyName);
            }

            return null;
        }

        /// <summary>
        /// Returns a <see cref="System.String" /> that represents this instance.
        /// </summary>
        /// <returns>
        /// A <see cref="System.String" /> that represents this instance.
        /// </returns>
        public override string ToString()
        {
            return this.Name + " = \"" + this.Value + "\"";
        }

        /// <summary>
        /// Validating dates
        /// </summary>
        /// <param name="propertyName">The name of the property to validate</param>
        /// <returns>A string describing the problem with the current value</returns>
        private string ValidateDates(string propertyName)
        {
            DateTime value;
            bool canParse = DateParseHelper.TryParseDate(this.Value, out value);
            
            if (value < this.MinDateValue || value > this.MaxDateValue)
            {
                return "The value must be within the limits specified by the minimum and maximum value, currently " + this.MinDateValue + " and " + this.MaxDateValue + " respectively.";
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
            double value;
            bool canParse = NumberParserHelper.TryParseDouble(this.Value, out value);

            if (!canParse)
            {
                return "This value must be numeric. Please verify that you're using the correct delimiter such as \",\" or \".\"";
            }

            //TODO: This currently accepts both "," and "." as delimeters for some culture settings. Which can lead to confusing number parsing

            if (value < this.MinValue || value > this.MaxValue)
            {
                return "The value must be within the limits specified by the minimum and maximum value, currently " + this.MinValue + " and " + this.MaxValue + " respectively.";
            }

            return null;
        }

        #endregion
    }
}
