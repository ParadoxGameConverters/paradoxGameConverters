using Converter.UI.Framework;
using Converter.UI.Settings;

namespace Converter.UI.ViewModels
{
    public class PreferencesViewModel : BaseViewModel
    {
        public PreferencesViewModel(ConverterOptions options, IView view, PreferenceCategory category)
            : base(view, category.FriendlyName, options)
        {
            this.Category = category;
        }

        public PreferenceCategory Category { get; private set; }
    }
}
