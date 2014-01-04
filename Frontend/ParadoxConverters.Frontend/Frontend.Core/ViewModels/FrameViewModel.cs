
using Caliburn.Micro;
using Frontend.Core.ViewModels.Interfaces;
namespace Frontend.Core.ViewModels
{
    public class FrameViewModel : StepConductorBase, IFrameViewModel
    {
        #region [ Fields ]

        private ILogViewModel logViewModel;

        #endregion 

        public FrameViewModel(IEventAggregator eventAggregator)
            : base(eventAggregator)
        {
        }

        #region [ Properties ]

        public ILogViewModel Log
        {
            get
            {
                return this.logViewModel ?? (this.logViewModel = new LogViewModel(this.EventAggregator));
            }
        }

        #endregion
    }
}
