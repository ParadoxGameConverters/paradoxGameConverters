using System.Collections.Generic;
using Frontend.Core.Model.Preferences.Interfaces;

namespace Frontend.Core.Model.Interfaces
{
    /// <summary>
    ///     A preference category defines a set of preferences by type or some other grouping.
    ///     An example can be "Economy", "Military" or whatever else the configuration files
    ///     find it useful to group by. Each of these preference categories will be displayed
    ///     as a preference view in the UI.
    /// </summary>
    public interface IPreferenceCategory
    {
        /// <summary>
        ///     Gets the list of preferences in this category.
        /// </summary>
        /// <value>
        ///     The preferences.
        /// </value>
        IList<IPreference> Preferences { get; }

        /// <summary>
        ///     Gets or sets the friendly name for this category.
        /// </summary>
        /// <value>
        ///     The name of the friendly.
        /// </value>
        string FriendlyName { get; set; }
    }
}