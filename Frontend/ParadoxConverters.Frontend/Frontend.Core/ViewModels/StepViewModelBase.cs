

namespace Frontend.Core.ViewModels
{
    using Caliburn.Micro;
    using Frontend.Core.Common;
    using Frontend.Core.Model.Interfaces;
    using Frontend.Core.Navigation;

    public abstract class StepViewModelBase : ViewModelBase, IStep
    {
        private IConverterOptions options;

        protected StepViewModelBase(IEventAggregator eventAggregator, IConverterOptions options)
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
        public abstract bool IsValid { get; }
    }
}
