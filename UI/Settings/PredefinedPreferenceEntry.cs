using Converter.UI.Enums;

namespace Converter.UI.Settings
{
    public class PredefinedPreferenceEntry : PreferenceEntry<string>
    {
        private bool isSelected;

        public PredefinedPreferenceEntry(string name, string friendlyName, string description, IPreference parent)
            : base(name, friendlyName, description, PreferenceType.PreDefined, parent)
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
    }
}
