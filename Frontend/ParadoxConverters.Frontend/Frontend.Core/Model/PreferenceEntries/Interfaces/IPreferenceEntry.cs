
using Frontend.Core.Model.Preferences.Interfaces;
namespace Frontend.Core.Model.PreferenceEntries.Interfaces
{
    public interface IPreferenceEntry
    {
        /// <summary>
        /// Gets or sets a value indicating whether this entry is selected or not
        /// </summary>
        bool IsSelected { get; set; }

        /// <summary>
        /// Gets the description.
        /// </summary>
        /// <value>
        /// The description.
        /// </value>
        string Description { get; set; }

        /// <summary>
        /// Gets the name of the friendly.
        /// </summary>
        /// <value>
        /// The name of the friendly.
        /// </value>
        string FriendlyName { get; set; }
    }

    /// <summary>
    /// Interface for a preference entry. A preference entry is one option/choice for a preference. 
    /// An IPreference object with multiple options has a list of IPreferenceEntries - one representing each choice the user can make.
    /// </summary>
    public interface IPreferenceEntry<T, TParent> : IPreferenceEntry
        where TParent : IPreference
    {
        /// <summary>
        /// Gets the name.
        /// </summary>
        /// <value>
        /// The name.
        /// </value>
        T Name { get; set; }

        /// <summary>
        /// Gets the parent.
        /// </summary>
        /// <value>
        /// The parent.
        /// </value>
        TParent Parent { get; set; }
    }
}
