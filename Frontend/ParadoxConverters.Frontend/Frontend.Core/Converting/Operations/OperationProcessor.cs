using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Threading;
using Caliburn.Micro;
using Frontend.Core.Logging;

namespace Frontend.Core.Converting.Operations
{
    public class OperationProcessor : DispatcherObject, IOperationProcessor
    {
        private readonly IEventAggregator eventAggregator;

        private readonly Dictionary<OperationResultState, Action<OperationResult, IOperationViewModel>>
            resultHandlershandle;

        public OperationProcessor(IEventAggregator eventAggregator)
        {
            this.eventAggregator = eventAggregator;
            resultHandlershandle = new Dictionary<OperationResultState, Action<OperationResult, IOperationViewModel>>
            {
                {OperationResultState.Success, HandleSuccess},
                {OperationResultState.Warning, HandleWarning},
                {OperationResultState.Error, HandleErrors},
                {OperationResultState.Canceled, HandleCancellation}
            };
        }

        public Task<AggregateOperationsResult> ProcessQueue(IEnumerable<IOperationViewModel> operations,
            CancellationToken token)
        {
            foreach (var operation in operations)
            {
                // Check if previous previousOperation failed. 
                var previousOperation = GetPreviousItem(operations, operation);
                if (token.IsCancellationRequested || !this.CanContinue(previousOperation))
                {
                    break;
                }

                operation.State = OperationState.InProgress;
                eventAggregator.PublishOnUIThread(new LogEntry(operation.Description, LogEntrySeverity.Info,
                    LogEntrySource.UI));

                var result = new OperationResult();

                try
                {
                    token.ThrowIfCancellationRequested();
                    result = operation.Process();

                    LogResultMessages(result);
                    resultHandlershandle[result.State](result, operation);
                }
                catch (OperationCanceledException)
                {
                    resultHandlershandle[OperationResultState.Canceled](result, operation);
                }
                catch (Exception e)
                {
                    eventAggregator.PublishOnUIThread(
                        new LogEntry(string.Format("Unhandled exception occurred: {0}", e.Message),
                            LogEntrySeverity.Error, LogEntrySource.UI));
                }
            }

            return Task.FromResult(CalculateResult(operations));
        }

        private static IOperationViewModel GetPreviousItem(IEnumerable<IOperationViewModel> operations, IOperationViewModel currentOperation)
        {
            // Todo: Redo this as extension method?
            var currentIndex = operations.ToList().IndexOf(currentOperation);
            var previousOperation = operations.ElementAt(currentIndex == 0 ? currentIndex : currentIndex - 1);
            return previousOperation;
        }

        private bool CanContinue(IOperationViewModel previousOperation)
        {
            var statesToContinueFrom = new List<OperationState>()
            {
                OperationState.CompleteWithWarnings,
                OperationState.Success,
                OperationState.NotStarted
            };

            return statesToContinueFrom.Contains(previousOperation.State);
        }

        private AggregateOperationsResult CalculateResult(IEnumerable<IOperationViewModel> operations)
        {
            var operationStates = from operation in operations select operation.State;

            if (operationStates.Contains(OperationState.CompleteWithErrors))
            {
                return AggregateOperationsResult.CompletedWithErrors;
            }
            if (operationStates.Contains(OperationState.Cancelled))
            {
                return AggregateOperationsResult.Canceled;
            }

            return AggregateOperationsResult.CompletedSuccessfully;
        }

        private void HandleCancellation(OperationResult result, IOperationViewModel operation)
        {
            eventAggregator.PublishOnUIThread(
                new LogEntry(string.Format("Operation {0} canceled.", operation.Description), LogEntrySeverity.Warning,
                    LogEntrySource.UI));
        }

        private void HandleSuccess(OperationResult result, IOperationViewModel operation)
        {
            operation.State = OperationState.Success;
            eventAggregator.PublishOnUIThread(
                new LogEntry(string.Format("{0} completed successfully.", operation.Description), LogEntrySeverity.Info,
                    LogEntrySource.UI));
        }

        private void HandleWarning(OperationResult result, IOperationViewModel operation)
        {
            operation.State = OperationState.CompleteWithWarnings;
            eventAggregator.PublishOnUIThread(
                new LogEntry(string.Format("{0} completed with warnings!", operation.Description),
                    LogEntrySeverity.Warning, LogEntrySource.UI));
        }

        private void HandleErrors(OperationResult result, IOperationViewModel operation)
        {
            operation.State = OperationState.CompleteWithErrors;
            eventAggregator.PublishOnUIThread(new LogEntry(string.Format("{0} failed!", operation.Description),
                LogEntrySeverity.Error, LogEntrySource.UI));
        }

        private void LogResultMessages(OperationResult result)
        {
            const int maxEntriesToShow = 1000;

            if (result.LogEntries.Count <= maxEntriesToShow)
            {
                this.PublishLogEntries(result.LogEntries);
            }
            else
            {
                var lastEntries =
                    result.LogEntries.AsEnumerable<LogEntry>().Skip(Math.Max(0, result.LogEntries.Count - maxEntriesToShow));

                this.PublishLogEntries(lastEntries);

                eventAggregator.PublishOnUIThread(new LogEntry("The converter returned " + result.LogEntries.Count + " lines.", LogEntrySeverity.Warning, LogEntrySource.UI));
                eventAggregator.PublishOnUIThread(new LogEntry("Only showing the last " + maxEntriesToShow, LogEntrySeverity.Warning, LogEntrySource.UI));
            }
        }

        private void PublishLogEntries(IEnumerable<LogEntry> logEntries)
        {
            foreach (var logEntry in logEntries)
            {
                eventAggregator.PublishOnUIThread(logEntry);
            }
        }
    }
}