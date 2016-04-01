using Caliburn.Micro;
using Frontend.Core.Helpers;
using Frontend.Core.Model.PreferenceEntries.Interfaces;
using Frontend.Core.Model.Preferences.Interfaces;

namespace Frontend.Core.Model.PreferenceEntries
{
    public abstract class PreferenceEntry<T, TParent> : PropertyChangedBase, IPreferenceEntry<T, TParent>
        where TParent : IPreference
    {
        private bool isSelected;
        private T name;

        /// <summary>
        ///     Gets the name.
        /// </summary>
        /// <value>
        ///     The name.
        /// </value>
        public T Name
        {
            get { return name; }

            set
            {
                if (NullableComparer.AreEqual(name, value))
                {
                    return;
                }

                name = value;
                NotifyOfPropertyChange(() => Name);
                //this.UpdateParentUserChoice();
            }
        }

        /// <summary>
        ///     Gets the name of the friendly.
        /// </summary>
        /// <value>
        ///     The name of the friendly.
        /// </value>
        public string FriendlyName { get; set; }

        /// <summary>
        ///     Gets the description.
        /// </summary>
        /// <value>
        ///     The description.
        /// </value>
        public string Description { get; set; }

        /// <summary>
        ///     Gets the parent.
        /// </summary>
        /// <value>
        ///     The parent.
        /// </value>
        public TParent Parent { get; set; }

        /// <summary>
        ///     Gets or sets a value indicating whether [is selected].
        /// </summary>
        /// <value>
        ///     <c>true</c> if [is selected]; otherwise, <c>false</c>.
        /// </value>
        public bool IsSelected
        {
            get { return isSelected; }

            set
            {
                if (isSelected == value)
                {
                    return;
                }

                isSelected = value;

                if (value)
                {
                    UpdateParentUserChoice();
                }

                NotifyOfPropertyChange(() => IsSelected);
            }
        }

        /// <summary>
        ///     Updates the parent user choice. (Sets the parent's "SelectedEntry" property to this.
        /// </summary>
        protected void UpdateParentUserChoice()
        {
            var parent = Parent as IPreference;
            parent.SelectedEntry = this;
        }
    }
}