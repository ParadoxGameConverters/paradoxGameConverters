using Frontend.Core.Model.PreferenceEntries.Interfaces;
using Frontend.Core.Model.Preferences.Interfaces;
using System;

namespace Frontend.Core.Model.PreferenceEntries
{
    public class DatePreferenceEntry : PreferenceEntry<DateTime, IDatePreference>, IDatePreferenceEntry
    {
    }
}
