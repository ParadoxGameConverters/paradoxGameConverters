using System.Collections.Generic;
using Converter.UI.Settings;
using Converter.UI.ViewModels;
using Converter.UI.Views;

namespace Converter.UI.Factories
{
    public class PreferenceViewModelFactory
    {
        public PreferenceViewModelFactory(ConverterOptions options)
        {
            this.ConverterOptions = options;
        }

        private ConverterOptions ConverterOptions { get; set; }

        public IList<PreferencesViewModel> BuildPreferenceViewModels()
        {
            var viewModels = new List<PreferencesViewModel>();

            foreach (PreferenceCategory category in this.ConverterOptions.ConfigurationProvider.PreferenceCategories)
            {
                var viewModel = new PreferencesViewModel(this.ConverterOptions, new PreferencesView(), category);
                viewModels.Add(viewModel);
            }

            return viewModels;
        }
    }
}
