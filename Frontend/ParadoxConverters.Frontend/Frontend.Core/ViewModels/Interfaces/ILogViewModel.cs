using Caliburn.Micro;
using Frontend.Core.Logging;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.ViewModels.Interfaces
{
    public interface ILogViewModel : IHandle<LogEntry>
    {
    }
}
