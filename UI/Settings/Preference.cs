using System.Collections.Generic;
using Converter.UI.Framework;

namespace Converter.UI.Settings
{
    public class Preference<T> : NotifiableBase, IPreference<T>
    {
        private IList<PreferenceEntry<T>> entries;
        private PreferenceEntry<T> userChoice;

        public string Name { get; set; }
        public string FriendlyName { get; set; }
        public string Description { get; set; }

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
    }
}
