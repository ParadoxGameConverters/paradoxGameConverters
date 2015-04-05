using Caliburn.Micro;
using Frontend.Core.Commands;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Converting
{
    public class CancelConvertingCommand : CommandBase
    {
        public CancelConvertingCommand(IEventAggregator eventAggregator)
            : base(eventAggregator)
        {
        }
    }
}
