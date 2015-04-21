using System;
using System.Threading;
using Caliburn.Micro;
using Frontend.Core.Commands;
using Frontend.Core.Logging;

namespace Frontend.Core.Converting
{
    public class CancelConvertingCommand : CommandBase
    {
        private readonly Func<CancellationTokenSource> tokenSourceFunc;

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
            EventAggregator.PublishOnUIThread(
                new LogEntry("Process canceled - will stop working once the current step is complete.",
                    LogEntrySeverity.Warning, LogEntrySource.UI));
            tokenSourceFunc().Cancel();
        }
    }
}