using System.Collections.Generic;

namespace Converter.UI.Settings
{
    /// <summary>
    /// A preference category is a set of IPreference objects that is related. 
    /// Each PreferenceCategory matches a <category> tag in Configuration.xml, and gets turned into a tab in the converter UI.
    /// </summary>
    public class PreferenceCategory
    {
        private IList<IPreference> preferences;

        /// <summary>
        /// Gets the list of preferences in this category.
        /// </summary>
        /// <value>
        /// The preferences.
        /// </value>
        public IList<IPreference> Preferences
        {
            get
            {
                return this.preferences ?? (this.preferences = new List<IPreference>());
            }
        }

        /// <summary>
        /// Gets or sets the friendly name for this category. 
        /// </summary>
        /// <value>
        /// The name of the friendly.
        /// </value>
        public string FriendlyName { get; set; }
    }
}
