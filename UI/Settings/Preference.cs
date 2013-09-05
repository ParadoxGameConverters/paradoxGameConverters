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
    public class Preference : NotifiableBase, IPreference, IDataErrorInfo
    {
        private IList<IPreferenceEntry> entries;
        private IPreferenceEntry selectedEntry;
        private string value;
        
        public string Name { get; set; }
        public string FriendlyName { get; set; }
        public string Description { get; set; }
        public double MinValue { get; set; }
        public double MaxValue { get; set; }
        public bool AllowDirectOverride { get; set; }

        public IList<IPreferenceEntry> Entries
        {
            get
            {
                return this.entries ?? (this.entries = new List<IPreferenceEntry>());
            }
        }

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

        public bool HasPreDefinedChoices
        {
            get
            {
                return this.Entries.Count > 0;
            }
        }

        public bool HasDirectlyEditableValue { get; set; }

        public bool IsNumeric
        {
            get
            {
                double value;
                return NumberParserHelper.TryParseDouble(this.Value, out value);
            }
        }

        #region [ Validation ]


        public string Error
        {
            get
            {
                return null;
            }
        }

        public string this[string name]
        {
            get
            {
                string result = this.OnValidateProperty(name);

                return result;
            }
        }

        private bool HasMinMaxValues
        {
            get
            {
                return this.MinValue >= double.MinValue && this.MaxValue >= double.MinValue;
            }
        }

        protected virtual string OnValidateProperty(string propertyName)
        {
            if (!this.HasMinMaxValues)
            {
                return null;
            }

            if (this.IsNumeric)
            {
                double value;
                NumberParserHelper.TryParseDouble(this.Value, out value);

                //TODO: This currently accepts both "," and "." as delimeters for some culture settings. Which can lead to confusing number parsing

                if (value < this.MinValue || value > this.MaxValue)
                {
                    return "The value must be within the limits specified by the minimum and maximum value, currently " + this.MinValue + " and " + this.MaxValue + " respectively.";
                }
            }
            else
            {
                return "This value must be numeric. Please verify that you're using the correct delimiter such as \",\" or \".\"";
            }

            return null;
        }

        public override string ToString()
        {
            return this.Name + " = \"" + this.Value + "\"";
        }

        #endregion
    }
}
