

namespace Frontend.Core.ViewModels
{
    using Caliburn.Micro;
    using Frontend.Core.Logging;
    using Frontend.Core.Model.Interfaces;
    using Frontend.Core.ViewModels;
    using Frontend.Core.ViewModels.Interfaces;

    public abstract class StepViewModelBase : ViewModelBase, IStep
    {
        private IConverterOptions options;

        public StepViewModelBase(IEventAggregator eventAggregator, IConverterOptions options)
            : base(eventAggregator)
        {
            this.options = options;
            //this.Load(null);
        }

        public IConverterOptions Options
        {
            get
            {
                return this.options;
            }
        }

        /// <summary>
        /// Tries to validate the current step. This will fail if important user input is missing or incorrect.
        /// </summary>
        /// <returns>True if validation succeeds, false if not.</returns>
        public abstract bool CanValidate();
    }
}
