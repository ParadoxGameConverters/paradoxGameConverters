

namespace Frontend.Core.ViewModels.Interfaces
{
    using Frontend.Core.Common;
    using Frontend.Core.Navigation;
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
