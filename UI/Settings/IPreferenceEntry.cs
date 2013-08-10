using Converter.UI.Enums;

namespace Converter.UI.Settings
{
    interface IPreferenceEntry
    {
        string Description { get; }
        string FriendlyName { get; }
        string Name { get; }
        IPreference Parent { get; }
        PreferenceType Type { get; }
    }
}
