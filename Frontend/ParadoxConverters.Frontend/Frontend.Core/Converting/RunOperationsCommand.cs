using Caliburn.Micro;
using Frontend.Core.Commands;
using Frontend.Core.Common;
using Frontend.Core.Converting.Operations;
using Frontend.Core.Model;
using Frontend.Core.Model.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Frontend.Core.Converting
{
    public class RunOperationsCommand : CommandBase
    {
        private IOperationProcessor processor;
        private IOperationProvider provider;
        private Action<int> reportProgress;
        private Func<CancellationTokenSource> tokenSourceFunc;

        public RunOperationsCommand(
            IEventAggregator eventAggregator, 
            IOperationProcessor processor, 
            IOperationProvider provider,
            Action<int> reportProgress,
            Func<CancellationTokenSource> tokenSourceFunc)
            : base(eventAggregator)
        {
            this.processor = processor;
            this.provider = provider;
            this.reportProgress = reportProgress;
            this.tokenSourceFunc = tokenSourceFunc;
        }

        protected override bool OnCanExecute(object parameter)
        {
            return true;
        }

        protected override void OnExecute(object parameter)
        {
            var progressIndicator = new Progress<int>(this.reportProgress);
            this.provider.Operations.ForEach(o => o.State = OperationState.NotStarted);
            this.processor.ProcessQueue(this.provider.Operations.Where(operation => operation.CanRun()), progressIndicator, this.tokenSourceFunc().Token);
        }
    }
}
