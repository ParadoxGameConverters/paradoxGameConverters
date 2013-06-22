using System.Collections.Generic;
using Converter.UI.Framework;
using Converter.UI.Enums;
using System.Text;
using System.Linq;
using System;

namespace Converter.UI.Settings
{
    public class Preference<T> : NotifiableBase, IPreference<T>
    {
        private IList<PreferenceEntry<T>> entries;
        private PreferenceEntry<T> userChoice;
        private Dictionary<PreferenceType, Func<string>> toStringLookup;

        public Preference(PreferenceType childType)
        {
            this.PreferenceEntryType = childType;
        }

        public string Name { get; set; }
        public string FriendlyName { get; set; }
        public string Description { get; set; }
        public PreferenceType PreferenceEntryType { get; private set; }

        public IList<PreferenceEntry<T>> Entries
        {
            get
            {
                return this.entries ?? (this.entries = new List<PreferenceEntry<T>>());
            }
        }

        public PreferenceEntry<T> UserChoice
        {
            get
            {
                return this.userChoice;
            }

            set
            {
                if (this.userChoice == value)
                {
                    return;
                }

                this.userChoice = value;
                this.RaisePropertyChanged("UserChoice");
            }
        }

        private Dictionary<PreferenceType, Func<string>> ToStringLookup
        {
            get
            {
                return this.toStringLookup ?? (this.toStringLookup = this.PopulateToStringLoookup());
            }
        }

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();

            sb.AppendLine(" # " + this.FriendlyName + ": " + this.Description);

            sb.AppendLine(this.ToStringLookup[this.PreferenceEntryType]().ToString());

            return sb.ToString();
        }

        private Dictionary<PreferenceType, Func<string>> PopulateToStringLoookup()
        {
            var lookup = new Dictionary<PreferenceType, Func<string>>();

            lookup.Add(PreferenceType.PlainString, this.PlainStringOutput);
            lookup.Add(PreferenceType.PreDefined, this.PredefinedOutput);
            lookup.Add(PreferenceType.Numerical, this.NumericalOutput);
            lookup.Add(PreferenceType.NumericPredefinedWithOverride, this.NumericPredefinedWithOverrideOutput);

            return lookup;
        }

        private string PlainStringOutput()
        {
            var preferenceAsString = this as Preference<string>;
            return preferenceAsString.Name + " = \"" + preferenceAsString.Entries.First() + "\"";
        }

        private string PredefinedOutput()
        {
            var preferenceAsString = this as Preference<string>;
            return preferenceAsString.Name + " = \"" + preferenceAsString.UserChoice.Name + "\"";
        }

        private string NumericalOutput()
        {
            var preferenceAsDouble = this as Preference<double>;
            var userChoice = preferenceAsDouble.UserChoice as NumericPreferenceEntry;
            return preferenceAsDouble.Name + " = \"" + userChoice.Value + "\"";
        }

        private string NumericPredefinedWithOverrideOutput()
        {
            var preferenceAsDouble = this as Preference<double>;
            return preferenceAsDouble.Name + " = \"" + preferenceAsDouble.UserChoice.Name + "\"";
        }
    }
}
