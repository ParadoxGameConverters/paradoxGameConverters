using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Windows.Input;
using Converter.UI.Settings;
using System.Windows.Threading;
using Converter.UI.Framework;

namespace Converter.UI.Commands
{
    /// <summary>
    /// Implementation of <c>ICommand</c> that allows for asynchronous operation.
    /// <remarks>
    /// Much of this was copied from http://kendoll.net/async_wpf_command
    /// </remarks>
    /// </summary>
    public class AsyncCommandBase : DispatcherObject, ICommand
    {
        private ConverterOptions options;
        BackgroundWorker worker;

        protected AsyncCommandBase(ConverterOptions options)
        {
            this.options = options;
            this.worker = new BackgroundWorker();
        }

        protected ConverterOptions Options
        {
            get
            {
                return this.options;
            }
        }

        /// <summary>
        /// When overridden in a derived class, performs operations in the UI thread
        /// before beginning the background operation.
        /// </summary>
        /// <param name="parameter">The parameter passed to the <c>Execute</c> method of the command.</param>
        protected virtual void BeforeExecute(object parameter) 
        { 
        }

        /// <summary>
        /// When overridden in a derived class, performs operations in a background
        /// thread when the <c>Execute</c> method is invoked.
        /// </summary>
        /// <param name="parameter">The paramter passed to the <c>Execute</c> method of the command.</param>
        protected virtual void OnExecute(object parameter) 
        { 
        }

        /// <summary>
        /// When overridden in a derived class, performs operations when the
        /// background execution has completed.
        /// </summary>
        /// <param name="parameter">The parameter passed to the <c>Execute</c> method of the command.</param>
        /// <param name="error">The error object that was thrown during the background operation, or null if no error was thrown.</param>
        protected virtual void AfterExecute(object parameter, Exception error) 
        { 
        }

        /// <summary>
        /// Let us use command manager for thread safety
        /// </summary>
        public event EventHandler CanExecuteChanged
        {
            add { CommandManager.RequerySuggested += value; }
            remove { CommandManager.RequerySuggested -= value; }
        }

        /// <summary>
        /// When overridden in a derived class, defines the method that determines whether the command can execute in its
        /// current state.
        /// </summary>
        /// <param name="parameter">
        /// Data used by the command. If the command does not require data to be passed,
        /// this object can be set to null.
        /// </param>
        /// <returns>True if this command can be executed; otherwise, false.</returns>
        public bool CanExecute(object parameter)
        {
            if (this.worker.IsBusy)
            {
                return false;
            }

            return this.OnCanExecute(parameter);
        }

        protected virtual bool OnCanExecute(object parameter)
        {
            return true;
        }

        /// <summary>
        /// Runs the command method in a background thread.
        /// </summary>
        /// <param name="parameter">
        /// Data used by the command. If the command does not require data to be passed,
        /// this object can be set to null.
        /// </param>
        public void Execute(object parameter)
        {
            this.worker.Dispose();
            this.worker = new BackgroundWorker();

            BeforeExecute(parameter);
            
            this.worker.DoWork += (s, e) =>
            {
                CommandManager.InvalidateRequerySuggested();
                OnExecute(parameter);
            };
            this.worker.RunWorkerCompleted += (s, e) =>
            {
                AfterExecute(parameter, e.Error);
                CommandManager.InvalidateRequerySuggested();
            };
            this.worker.RunWorkerAsync();
        }

        /// <summary>
        /// Marshalls the method.
        /// </summary>
        /// <param name="action">The action.</param>
        /// <param name="priority">The priority.</param>
        protected void MarshallMethod(Action action, DispatcherPriority priority)
        {
            if (!this.Dispatcher.CheckAccess())
            {
                this.Dispatcher.Invoke(action, priority);
                return;
            }

            action();
        }
    }
}
