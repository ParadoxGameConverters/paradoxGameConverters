using Caliburn.Micro;
using Frontend.Core.ViewModels;
using Frontend.Core.ViewModels.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Commands
{
    public class MoveCommand : CommandBase
    {
        private IStepConductorBase conductorViewModel;

        public MoveCommand(IEventAggregator eventAggregator, IStepConductorBase conductorViewModel)
            : base(eventAggregator)
        {
            this.conductorViewModel = conductorViewModel;
        }

        protected override bool OnCanExecute(object parameter)
        {
            switch ((Direction)parameter)
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
            this.conductorViewModel.Move((Direction)parameter);
        }
    }
}
