using Converter.UI.Enums;

namespace Converter.UI.Settings
{
    /// <summary>
    /// Interface for a preference entry. A preference entry is one option/choice for a preference. 
    /// An IPreference object with multiple options has a list of IPreferenceEntries - one representing each choice the user can make.
    /// </summary>
    public interface IPreferenceEntry
    {
        /// <summary>
        /// Gets the description.
        /// </summary>
        /// <value>
        /// The description.
        /// </value>
        string Description { get; }

        /// <summary>
        /// Gets the name of the friendly.
        /// </summary>
        /// <value>
        /// The name of the friendly.
        /// </value>
        string FriendlyName { get; }

        /// <summary>
        /// Gets the name.
        /// </summary>
        /// <value>
        /// The name.
        /// </value>
        string Name { get; }

        /// <summary>
        /// Gets the parent.
        /// </summary>
        /// <value>
        /// The parent.
        /// </value>
        IPreference Parent { get; }
    }
}
