using Caliburn.Micro;
using Frontend.Core.Common;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.Navigation;

namespace Frontend.Core.ViewModels
{
    public abstract class StepViewModelBase : ViewModelBase, IStep
    {
        protected StepViewModelBase(IEventAggregator eventAggregator, IConverterOptions options)
            : base(eventAggregator)
        {
            Options = options;
            //this.Load(null);
        }

        public IConverterOptions Options { get; private set; }

        /// <summary>
        ///     Tries to validate the current step. This will fail if important user input is missing or incorrect.
        /// </summary>
        /// <returns>True if validation succeeds, false if not.</returns>
        public abstract bool IsValid { get; }
    }
}