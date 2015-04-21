using Caliburn.Micro;
using Frontend.Core.Commands;
using Frontend.Core.Converting.Operations;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Frontend.Core.Logging;

namespace Frontend.Core.Converting
{
    public class CancelConvertingCommand : CommandBase
    {
        private Func<CancellationTokenSource> tokenSourceFunc;

        public CancelConvertingCommand(IEventAggregator eventAggregator, Func<CancellationTokenSource> tokenSourceFunc)
            : base(eventAggregator)
        {
            this.tokenSourceFunc = tokenSourceFunc;
        }

        protected override bool OnCanExecute(object parameter)
        {
            return !tokenSourceFunc().IsCancellationRequested;
        }

        protected override void OnExecute(object parameter)
        {
            this.EventAggregator.PublishOnUIThread(new LogEntry("Process canceled - will stop working once the current step is complete.", LogEntrySeverity.Warning, LogEntrySource.UI));
            this.tokenSourceFunc().Cancel();
        }
    }
}
