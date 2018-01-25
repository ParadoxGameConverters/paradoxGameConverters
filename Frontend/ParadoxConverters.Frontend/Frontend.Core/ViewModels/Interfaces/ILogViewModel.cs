using Caliburn.Micro;
using Frontend.Core.Logging;

namespace Frontend.Core.ViewModels.Interfaces
{
    public interface ILogViewModel : IHandle<LogEntry>
    {
    }
}