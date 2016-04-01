using System;
using System.ComponentModel;
using System.Windows.Input;
using System.Windows.Threading;
using Caliburn.Micro;
using Frontend.Core.Model.Interfaces;
using Action = System.Action;

namespace Frontend.Core.Commands
{
    /// <summary>
    ///     Implementation of <c>ICommand</c> that allows for asynchronous operation.
    ///     <remarks>
    ///         Much of this was copied from http://kendoll.net/async_wpf_command
    ///     </remarks>
    /// </summary>
    public class OldAsyncCommandBase : DispatcherObject, ICommand, IDisposable
    {
        private BackgroundWorker worker;

        /// <summary>
        ///     Initializes a new instance of the <see cref="OldAsyncCommandBase" /> class.
        /// </summary>
        /// <param name="eventAggregator"></param>
        /// <param name="options"></param>
        protected OldAsyncCommandBase(IEventAggregator eventAggregator, IConverterOptions options)
        {
            Options = options;
            worker = new BackgroundWorker();
            EventAggregator = eventAggregator;
        }

        protected IConverterOptions Options { get; private set; }
        protected IEventAggregator EventAggregator { get; private set; }

        /// <summary>
        ///     Let us use command manager for thread safety
        /// </summary>
        public event EventHandler CanExecuteChanged
        {
            add { CommandManager.RequerySuggested += value; }
            remove { CommandManager.RequerySuggested -= value; }
        }

        /// <summary>
        ///     When overridden in a derived class, defines the method that determines whether the command can execute in its
        ///     current state.
        /// </summary>
        /// <param name="parameter">
        ///     Data used by the command. If the command does not require data to be passed,
        ///     this object can be set to null.
        /// </param>
        /// <returns>True if this command can be executed; otherwise, false.</returns>
        public bool CanExecute(object parameter)
        {
            if (worker.IsBusy)
            {
                return false;
            }

            return OnCanExecute(parameter);
        }

        /// <summary>
        ///     Runs the command method in a background thread.
        /// </summary>
        /// <param name="parameter">
        ///     Data used by the command. If the command does not require data to be passed,
        ///     this object can be set to null.
        /// </param>
        public void Execute(object parameter)
        {
            worker.Dispose();
            worker = new BackgroundWorker();

            BeforeExecute(parameter);

            worker.DoWork += (s, e) =>
            {
                CommandManager.InvalidateRequerySuggested();
                OnExecute(parameter);
            };
            worker.RunWorkerCompleted += (s, e) =>
            {
                AfterExecute(parameter, e.Error);
                CommandManager.InvalidateRequerySuggested();
            };
            worker.RunWorkerAsync();
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool isDisposing)
        {
            if (isDisposing)
            {
                worker.Dispose();
            }
        }

        /// <summary>
        ///     When overridden in a derived class, performs operations in the UI thread
        ///     before beginning the background operation.
        /// </summary>
        /// <param name="parameter">The parameter passed to the <c>Execute</c> method of the command.</param>
        protected virtual void BeforeExecute(object parameter)
        {
        }

        /// <summary>
        ///     When overridden in a derived class, performs operations in a background
        ///     thread when the <c>Execute</c> method is invoked.
        /// </summary>
        /// <param name="parameter">The paramter passed to the <c>Execute</c> method of the command.</param>
        protected virtual void OnExecute(object parameter)
        {
        }

        /// <summary>
        ///     When overridden in a derived class, performs operations when the
        ///     background execution has completed.
        /// </summary>
        /// <param name="parameter">The parameter passed to the <c>Execute</c> method of the command.</param>
        /// <param name="error">The error object that was thrown during the background operation, or null if no error was thrown.</param>
        protected virtual void AfterExecute(object parameter, Exception error)
        {
        }

        protected virtual bool OnCanExecute(object parameter)
        {
            return true;
        }

        /// <summary>
        ///     Marshalls the method.
        /// </summary>
        /// <param name="action">The action.</param>
        /// <param name="priority">The priority.</param>
        protected void MarshallMethod(Action action, DispatcherPriority priority)
        {
            if (!Dispatcher.CheckAccess())
            {
                Dispatcher.Invoke(action, priority);
                return;
            }

            action();
        }
    }
}