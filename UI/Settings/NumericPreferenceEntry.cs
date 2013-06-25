using Converter.UI.Enums;

namespace Converter.UI.Settings
{
    public class NumericPreferenceEntry : PreferenceEntry<double>
    {
        private double value;

        public NumericPreferenceEntry(string name, string friendlyName, string description, double minValue, double maxValue, double defaultValue, IPreference parent)
            : base(name, friendlyName, description, PreferenceType.Numerical, parent)
        {
            this.MinValue = minValue;
            this.MaxValue = maxValue;
            this.DefaultValue = defaultValue;
            this.Value = defaultValue;
        }

        public double MinValue { get; private set; }
        public double MaxValue { get; private set; }
        public double DefaultValue { get; private set; }

        public double Value
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

                this.UpdateParentUserChoice();
            }
        }

        protected override string OnValidateProperty(string propertyName)
        {
            if (propertyName.Equals("Value"))
            {
                if (this.Value < this.MinValue || this.Value > this.MaxValue)
                {
                    //TODO: Update this to refer to the current configuration file, perhaps even with line numbers
                    return "The value must be between " + this.MinValue + " and " + this.MaxValue + ". You can change these values in Configuration.xml."; 
                }
            }

            return null;
        }
    }
}
