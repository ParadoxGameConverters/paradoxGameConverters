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
        private IConverterOptions options;
        private IOperationProcessor processor;
        private IOperationProvider provider;
        private Action<int> reportProgress;
        private CancellationToken token;

        public RunOperationsCommand(
            IEventAggregator eventAggregator, 
            IConverterOptions options, 
            IOperationProcessor processor, 
            IOperationProvider provider,
            Action<int> reportProgress,
            CancellationToken token)
            : base(eventAggregator)
        {
            this.options = options;
            this.processor = processor;
            this.provider = provider;
            this.reportProgress = reportProgress;
            this.token = token;
        }

        protected override bool OnCanExecute(object parameter)
        {
            return this.provider.Operations.All(operation => operation.CanRun());
        }

        protected override void OnExecute(object parameter)
        {
            var progressIndicator = new Progress<int>(this.reportProgress);
            this.provider.Operations.ForEach(o => o.State = OperationState.NotStarted);
            this.processor.ProcessQueue(this.provider.Operations, progressIndicator, this.token);
        }
    }
}
