using Converter.UI.Enums;

namespace Converter.UI.Settings
{
    public class StringPreferenceEntry : PreferenceEntry<string>
    {
        private bool isSelected;

        public StringPreferenceEntry(string name, string friendlyName, string description, IPreference parent) //, IList<string> predefinedChoices
            : base(name, friendlyName, description, PreferenceType.PreDefined, parent) // , predefinedChoices
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
                ////if (Comparer<T>.Equals(this.isSelected, value))
                ////{
                ////    return;
                ////}

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
