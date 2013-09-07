using System.Collections.Generic;
using Converter.UI.Settings;
using Converter.UI.ViewModels;
using Converter.UI.Views;

namespace Converter.UI.Factories
{
    /// <summary>
    /// This factory is used to create a list of PreferenceViewModels based on the Categories available in the ConverterOptions.ConfigurationProvider.PreferenceCategories collection
    /// </summary>
    public class PreferenceViewModelFactory
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="PreferenceViewModelFactory"/> class.
        /// </summary>
        /// <param name="options">The options.</param>
        public PreferenceViewModelFactory(ConverterOptions options)
        {
            this.ConverterOptions = options;
        }

        /// <summary>
        /// Gets or sets the converter options.
        /// </summary>
        /// <value>
        /// The converter options.
        /// </value>
        private ConverterOptions ConverterOptions { get; set; }

        /// <summary>
        /// Builds the preference view models.
        /// </summary>
        /// <returns></returns>
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
