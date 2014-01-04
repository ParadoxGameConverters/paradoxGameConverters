

namespace Frontend.Core.ViewModels
{
    using Caliburn.Micro;
    using Frontend.Core.ViewModels;
    using Frontend.Core.ViewModels.Interfaces;

    public abstract class StepViewModelBase : ViewModelBase, IStep
    {
        public StepViewModelBase(IEventAggregator eventAggregator)
            : base(eventAggregator)
        {
        }
    }
}
