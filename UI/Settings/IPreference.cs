using System.Collections.Generic;
using System.ComponentModel;

namespace Converter.UI.Settings
{
    public interface IPreference
    {
        string Name { get; set; }
        string FriendlyName { get; set; }
        string Description { get; set; }
        bool HasPreDefinedChoices { get; }
        bool HasDirectlyEditableValue { get; set; }
        bool IsNumeric { get; }
        IList<IPreferenceEntry> Entries { get; }
        double MinValue { get; set; }
        double MaxValue { get; set; }
        string Value { get; set; }
        IPreferenceEntry SelectedEntry { get; set; }
    }
}
