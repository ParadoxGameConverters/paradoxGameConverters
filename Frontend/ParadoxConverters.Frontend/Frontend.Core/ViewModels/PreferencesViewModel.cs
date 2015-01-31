using Caliburn.Micro;
using Frontend.Core.Model.Interfaces;

namespace Frontend.Core.ViewModels
{
    public class PreferencesViewModel : StepViewModelBase
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="PreferencesViewModel"/> class.
        /// </summary>
        /// <param name="eventAggregator">The event aggregator.</param>
        /// <param name="options">The options.</param>
        /// <param name="category">The category.</param>
        public PreferencesViewModel(IEventAggregator eventAggregator, IConverterOptions options, IPreferenceCategory category)
            : base(eventAggregator, options)
        {
            this.Category = category;
        }

        /// <summary>
        /// Gets the category for this viewmodel
        /// </summary>
        /// <value>
        /// The category.
        /// </value>
        public IPreferenceCategory Category { get; private set; }

        /// <summary>
        /// Tries to validate the current step. This will fail if important user input is missing or incorrect.
        /// </summary>
        /// <returns>True if validation succeeds, false if not.</returns>
        public override bool IsValid
        {
            get
            {
                return true;
            }
        }
    }
}
