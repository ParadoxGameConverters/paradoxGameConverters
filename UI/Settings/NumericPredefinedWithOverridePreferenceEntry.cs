using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Converter.UI.Enums;

namespace Converter.UI.Settings
{
    public class NumericPredefinedWithOverridePreferenceEntry : PreferenceEntry<double>
    {
        private bool isSelected;

        public NumericPredefinedWithOverridePreferenceEntry(string name, string friendlyName, string description, IPreference parent)
            : base(name, friendlyName, description, PreferenceType.NumericPredefinedWithOverride, parent)
        {
        }

        public bool IsSelected
        {
            get
            {
                return this.isSelected;
            }

            set
            {
                if (this.isSelected == value)
                {
                    return;
                }

                this.isSelected = value;

                if (value)
                {
                    this.UpdateParentUserChoice();
                }

                this.RaisePropertyChanged("IsSelected");
            }
        }

        public override string ToString()
        {
            return this.Name.ToString();
        }
    }
}
