using System;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Frontend.Core.Converting.Operations;
using Action = System.Action;

namespace Frontend.Core.Converting
{
    public class RunOperationsCommand : AsyncCommandBase
    {
        private readonly Action _afterCompletion;
        private readonly Action _beforeStart;
        private readonly IOperationProcessor processor;
        private readonly IOperationProvider provider;
        private readonly Func<CancellationTokenSource> tokenSourceFunc;

        public RunOperationsCommand(
            IOperationProcessor processor,
            IOperationProvider provider,
            Action beforeStart,
            Action afterCompletion,
            Func<CancellationTokenSource> tokenSourceFunc)
        {
            this.processor = processor;
            this.provider = provider;
            _beforeStart = beforeStart;
            _afterCompletion = afterCompletion;
            this.tokenSourceFunc = tokenSourceFunc;
        }

        public override bool CanExecute(object parameter)
        {
            return provider.Operations.All(operation => operation.State != OperationState.InProgress);
        }

        public override async Task ExecuteAsync(object parameter)
        {
            _beforeStart();
            provider.Operations.ForEach(o => o.State = OperationState.NotStarted);
            var result =
                await
                    Task.Run(
                        () =>
                            processor.ProcessQueue(provider.Operations.Where(operation => operation.CanRun()),
                                tokenSourceFunc().Token));
            _afterCompletion();
        }
    }
}