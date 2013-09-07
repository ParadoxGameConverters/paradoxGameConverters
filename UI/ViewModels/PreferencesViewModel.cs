using Converter.UI.Framework;
using Converter.UI.Settings;

namespace Converter.UI.ViewModels
{
    /// <summary>
    /// The preferences viewmodel is the backing viewmodel for the preferences view. There may be many of both, depending on configuration.xml. 
    /// One view + viewmodel pair will be created per category, and used to display the set of preferences in the aforementioned category.
    /// </summary>
    public class PreferencesViewModel : BaseViewModel
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="PreferencesViewModel"/> class.
        /// </summary>
        /// <param name="options">The options.</param>
        /// <param name="view">The view.</param>
        /// <param name="category">The category.</param>
        public PreferencesViewModel(ConverterOptions options, IView view, PreferenceCategory category)
            : base(view, category.FriendlyName, options)
        {
            this.Category = category;
        }

        /// <summary>
        /// Gets the category for this viewmodel
        /// </summary>
        /// <value>
        /// The category.
        /// </value>
        public PreferenceCategory Category { get; private set; }
    }
}
