using System.ComponentModel;
using Converter.UI.Enums;
using Converter.UI.Framework;

namespace Converter.UI.Settings
{
    public class PreferenceEntry : NotifiableBase, IPreferenceEntry
    {
        private bool isSelected;

        public PreferenceEntry(string name, string friendlyName, string description, IPreference parent)
        {
            this.Name = name;
            this.Description = description;
            this.FriendlyName = friendlyName;
            this.Parent = parent;
        }

        public string Name { get; private set; }
        public string FriendlyName { get; private set; }
        public string Description { get; private set; }
        public IPreference Parent { get; private set; }

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

        protected void UpdateParentUserChoice()
        {
            var parent = this.Parent as IPreference;
            parent.SelectedEntry = this;
        }
    }
}
