using Caliburn.Micro;
using Frontend.Core.Model.PreferenceEntries.Interfaces;
using Frontend.Core.Model.Preferences.Interfaces;
using System.Collections.Generic;

namespace Frontend.Core.Model.PreferenceEntries
{
    // StringListPreference stores a List<string> but each PreferenceEntry is just a string. 
    public class StringListPreferenceEntry : PreferenceEntry<string, IStringListPreference>, IStringListPreferenceEntry
    {
        /// <summary>
        ///     Adds or removes the selected entry from the parent preference's Value list.
        /// </summary>
        protected override void UpdateParentUserChoice()
        {
            var parent = Parent as IStringListPreference;

            if (parent.Value == null) // it might not be initialised, do so.
            {
                parent.Value = new List<string>();
            }

            if (this.IsSelected)
            {
                parent.Value.Add(this.Name);
            }
            else
            {
                parent.Value.Remove(this.Name);
            }
        }
    }
}