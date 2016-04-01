using Caliburn.Micro;
using Frontend.Core.Navigation;
using Frontend.Core.ViewModels.Interfaces;

namespace Frontend.Core.Commands
{
    public class MoveCommand : CommandBase
    {
        private readonly IStepConductorBase conductorViewModel;

        public MoveCommand(IEventAggregator eventAggregator, IStepConductorBase conductorViewModel)
            : base(eventAggregator)
        {
            this.conductorViewModel = conductorViewModel;
        }

        protected override bool OnCanExecute(object parameter)
        {
            switch ((Direction) parameter)
            {
                case Direction.Backward:
                    return conductorViewModel.CanMoveBackward;

                case Direction.Forward:
                    return conductorViewModel.CanMoveForward;
            }

            return false;
        }

        protected override void OnExecute(object parameter)
        {
            conductorViewModel.Move((Direction) parameter);
        }
    }
}