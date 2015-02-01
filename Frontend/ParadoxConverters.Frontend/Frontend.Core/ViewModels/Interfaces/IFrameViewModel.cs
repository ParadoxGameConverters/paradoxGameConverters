using Caliburn.Micro;
using Frontend.Core.Events.EventArgs;

namespace Frontend.Core.ViewModels.Interfaces
{
    public interface IFrameViewModel : IStepConductorBase, IHandle<PreferenceStepOperationArgs>
    {
        ILogViewModel Log { get; }
    }
}
