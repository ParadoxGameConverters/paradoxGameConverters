

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
        }

        public IConverterOptions Options
        {
            get
            {
                return this.options;
            }
        }
    }
}
