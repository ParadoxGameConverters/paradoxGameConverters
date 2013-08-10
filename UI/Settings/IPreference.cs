using System.Collections.Generic;

namespace Converter.UI.Settings
{
    public interface IPreference
    {
        string Name { get; set; }
        string FriendlyName { get; set; }
        string Description { get; set; }
    }

    public interface IPreference<T> : IPreference
    {
        IList<PreferenceEntry<T>> Entries { get; }
    }
}
