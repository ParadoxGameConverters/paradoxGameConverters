
using Caliburn.Micro;
using Frontend.Core.Commands;
using Frontend.Core.ViewModels.Interfaces;
using System.Windows.Input;
namespace Frontend.Core.ViewModels
{
    public class FrameViewModel : StepConductorBase, IFrameViewModel
    {
        #region [ Fields ]

        private ILogViewModel logViewModel;
        private ICommand moveCommand;

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

        public ICommand MoveCommand
        {
            get
            {
                return this.moveCommand ?? (this.moveCommand = new MoveCommand(this.EventAggregator, this));
            }
        }

        #endregion
    }
}
