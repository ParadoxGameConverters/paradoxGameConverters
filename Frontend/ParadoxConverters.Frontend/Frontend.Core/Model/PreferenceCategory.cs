using System.Collections.Generic;
using Caliburn.Micro;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.Model.Preferences.Interfaces;

namespace Frontend.Core.Model
{
    /// <summary>
    ///     A preference category is a set of IPreference objects that is related.
    ///     Each PreferenceCategory matches a
    ///     <category> tag in Configuration.xml, and gets turned into a view in the converter UI.
    /// </summary>
    public class PreferenceCategory : PropertyChangedBase, IPreferenceCategory
    {
        private IList<IPreference> preferences;

        /// <summary>
        ///     Gets the list of preferences in this category.
        /// </summary>
        /// <value>
        ///     The preferences.
        /// </value>
        public IList<IPreference> Preferences
        {
            get { return preferences ?? (preferences = new List<IPreference>()); }
        }

        /// <summary>
        ///     Gets or sets the friendly name for this category.
        /// </summary>
        /// <value>
        ///     The name of the friendly.
        /// </value>
        public string FriendlyName { get; set; }
    }
}