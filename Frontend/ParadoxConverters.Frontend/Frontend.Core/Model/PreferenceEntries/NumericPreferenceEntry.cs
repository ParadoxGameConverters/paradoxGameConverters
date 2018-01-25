using Frontend.Core.Model.PreferenceEntries.Interfaces;
using Frontend.Core.Model.Preferences.Interfaces;

namespace Frontend.Core.Model.PreferenceEntries
{
    public class NumericPreferenceEntry : PreferenceEntry<double, INumericPreference>, INumericPreferenceEntry
    {
    }
}