using System;
using Frontend.Core.Model.Preferences.Interfaces;

namespace Frontend.Core.Model.PreferenceEntries.Interfaces
{
    public interface IDatePreferenceEntry : IPreferenceEntry<DateTime, IDatePreference>
    {
    }
}