using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Converter.UI.Enums;
using System.ComponentModel;

namespace Converter.UI.Settings
{
    public class PlainStringPreferenceEntry : PreferenceEntry<string>
    {
        private string value;

        public PlainStringPreferenceEntry(string name, string friendlyName, string description, IPreference parent, string defaultValue)
            : base(name, friendlyName, description, PreferenceType.PlainString, parent)
        {
            this.DefaultValue = defaultValue;
            this.Value = defaultValue;
        }

        public string DefaultValue { get; private set; }

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

        public override string ToString()
        {
            return this.Value;
        }
    }
}
