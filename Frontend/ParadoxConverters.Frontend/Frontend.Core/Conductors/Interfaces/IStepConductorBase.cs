

namespace Frontend.Core.ViewModels.Interfaces
{
    using Caliburn.Micro;
    using Frontend.Core.Events.EventArgs;
    using System;
    using System.Collections.ObjectModel;

    public interface IStepConductorBase : IViewModelBase
    {
        ObservableCollection<IStep> Steps { get; }

        IStep CurrentStep { get; }

        //void MoveToStep(IStep step);
        void Move(Direction direction);

        bool CanMoveForward { get; }

        bool CanMoveBackward { get; }
    }
}
