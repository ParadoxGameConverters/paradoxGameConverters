using System.Collections.ObjectModel;
using Frontend.Core.Common;
using Frontend.Core.Navigation;

namespace Frontend.Core.ViewModels.Interfaces
{
    public interface IStepConductorBase : IViewModelBase
    {
        ObservableCollection<IStep> Steps { get; }
        IStep CurrentStep { get; }
        bool CanMoveForward { get; }
        bool CanMoveBackward { get; }
        //void MoveToStep(IStep step);
        void Move(Direction direction);
    }
}