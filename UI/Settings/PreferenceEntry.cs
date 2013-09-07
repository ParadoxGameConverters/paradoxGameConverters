using System.ComponentModel;
using Converter.UI.Enums;
using Converter.UI.Framework;

namespace Converter.UI.Settings
{
    /// <summary>
    /// Implementation of IPreferenceEntry. Basically, this object contains the properties for each choice in a list of pre-defined options for one particular IPreference.
    /// </summary>
    public class PreferenceEntry : NotifiableBase, IPreferenceEntry
    {
        private bool isSelected;

        /// <summary>
        /// Initializes a new instance of the <see cref="PreferenceEntry"/> class.
        /// </summary>
        /// <param name="name">The name.</param>
        /// <param name="friendlyName">Name of the friendly.</param>
        /// <param name="description">The description.</param>
        /// <param name="parent">The parent.</param>
        public PreferenceEntry(string name, string friendlyName, string description, IPreference parent)
        {
            this.Name = name;
            this.Description = description;
            this.FriendlyName = friendlyName;
            this.Parent = parent;
        }

        /// <summary>
        /// Gets the name.
        /// </summary>
        /// <value>
        /// The name.
        /// </value>
        public string Name { get; private set; }

        /// <summary>
        /// Gets the name of the friendly.
        /// </summary>
        /// <value>
        /// The name of the friendly.
        /// </value>
        public string FriendlyName { get; private set; }

        /// <summary>
        /// Gets the description.
        /// </summary>
        /// <value>
        /// The description.
        /// </value>
        public string Description { get; private set; }

        /// <summary>
        /// Gets the parent.
        /// </summary>
        /// <value>
        /// The parent.
        /// </value>
        public IPreference Parent { get; private set; }

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

                this.RaisePropertyChanged("IsSelected");
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
