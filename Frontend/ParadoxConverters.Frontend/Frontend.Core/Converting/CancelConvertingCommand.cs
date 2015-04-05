using Caliburn.Micro;
using Frontend.Core.Commands;
using Frontend.Core.Converting.Operations;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Frontend.Core.Converting
{
    public class CancelConvertingCommand : CommandBase
    {
        private CancellationTokenSource tokenSource;

        public CancelConvertingCommand(IEventAggregator eventAggregator, CancellationTokenSource tokenSource)
            : base(eventAggregator)
        {
            this.tokenSource = tokenSource;
        }

        protected override bool OnCanExecute(object parameter)
        {
            return !tokenSource.IsCancellationRequested;
        }

        protected override void OnExecute(object parameter)
        {
            this.tokenSource.Cancel();
        }
    }
}
