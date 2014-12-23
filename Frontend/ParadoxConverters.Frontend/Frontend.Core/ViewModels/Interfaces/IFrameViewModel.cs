using Caliburn.Micro;
using Frontend.Core.Events.EventArgs;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.ViewModels.Interfaces
{
    public interface IFrameViewModel : IStepConductorBase, IHandle<PreferenceStepOperationArgs>
    {
        ILogViewModel Log { get; }
    }
}
