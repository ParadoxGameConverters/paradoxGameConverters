using Frontend.Core.Model.PreferenceEntries.Interfaces;
using Frontend.Core.Model.Preferences.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Model.PreferenceEntries
{
    public class NumericPreferenceEntry : PreferenceEntry<double, INumericPreference>, INumericPreferenceEntry
    {
    }
}
