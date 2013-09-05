using Converter.UI.Enums;

namespace Converter.UI.Settings
{
    public interface IPreferenceEntry
    {
        string Description { get; }
        string FriendlyName { get; }
        string Name { get; }
        IPreference Parent { get; }
    }
}
