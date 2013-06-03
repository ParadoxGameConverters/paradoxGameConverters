using System.Collections.Generic;

namespace Converter.UI.Settings
{
    public class PreferenceCategory
    {
        private IList<IPreference> preferences;

        public IList<IPreference> Preferences
        {
            get
            {
                return this.preferences ?? (this.preferences = new List<IPreference>());
            }
        }

        public string FriendlyName { get; set; }
    }
}
