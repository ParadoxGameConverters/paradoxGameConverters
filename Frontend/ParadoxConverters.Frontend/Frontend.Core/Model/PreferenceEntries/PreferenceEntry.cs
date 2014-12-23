using Caliburn.Micro;
using Frontend.Core.Helpers;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.Model.PreferenceEntries.Interfaces;
using Frontend.Core.Model.Preferences.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Model.PreferenceEntries
{
    public abstract class PreferenceEntry<T, TParent> : PropertyChangedBase, IPreferenceEntry<T, TParent>
        where TParent : IPreference
    {
        private bool isSelected;

        private T name;

        /// <summary>
        /// Gets the name.
        /// </summary>
        /// <value>
        /// The name.
        /// </value>
        public T Name
        {
            get
            {
                return this.name;
            }

            set
            {
                if (NullableComparer.AreEqual(this.name, value))
                {
                    return;
                }

                this.name = value;
                this.NotifyOfPropertyChange(() => this.Name);
                //this.UpdateParentUserChoice();
            }
        }

        /// <summary>
        /// Gets the name of the friendly.
        /// </summary>
        /// <value>
        /// The name of the friendly.
        /// </value>
        public string FriendlyName { get; set; }

        /// <summary>
        /// Gets the description.
        /// </summary>
        /// <value>
        /// The description.
        /// </value>
        public string Description { get; set; }

        /// <summary>
        /// Gets the parent.
        /// </summary>
        /// <value>
        /// The parent.
        /// </value>
        public TParent Parent { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether [is selected].
        /// </summary>
        /// <value>
        ///   <c>true</c> if [is selected]; otherwise, <c>false</c>.
        /// </value>
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

                this.NotifyOfPropertyChange(() => this.IsSelected);
            }
        }

        /// <summary>
        /// Updates the parent user choice. (Sets the parent's "SelectedEntry" property to this.
        /// </summary>
        protected void UpdateParentUserChoice()
        {
            var parent = this.Parent as IPreference;
            parent.SelectedEntry = this;
        }
    }
}
