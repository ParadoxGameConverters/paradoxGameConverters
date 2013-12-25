using System;
using System.Windows.Input;
using Converter.UI.Settings;
using System.Windows.Threading;

namespace Converter.UI.Commands
{
    /// <summary>
    /// Base class for various commands
    /// </summary>
    public abstract class CommandBase : DispatcherObject, ICommand
    {
        private ConverterOptions options;

        /// <summary>
        /// Initializes a new instance of the <see cref="CommandBase"/> class.
        /// </summary>
        /// <param name="options">The options.</param>
        protected CommandBase(ConverterOptions options)
        {
            this.options = options;
        }

        /// <summary>
        /// Occurs when changes occur that affect whether or not the command should execute.
        /// <remarks>A quite inconvenient canexecute handler, as it tends to trigger several canexecute checks a second. Not enough to cause performance problems, but hardly optimal</remarks>
        /// </summary>
        public event EventHandler CanExecuteChanged
        {
            add { CommandManager.RequerySuggested += value; }

            remove { CommandManager.RequerySuggested -= value; }
        }

        /// <summary>
        /// Gets the converter options.
        /// </summary>
        /// <value>
        /// The options.
        /// </value>
        protected ConverterOptions Options
        {
            get
            {
                return this.options;
            }
        }

        /// <summary>
        /// Defines the method that determines whether the command can execute in its current state.
        /// </summary>
        /// <param name="parameter">Data used by the command.  If the command does not require data to be passed, this object can be set to null.</param>
        /// <returns>
        /// true if this command can be executed; otherwise, false.
        /// </returns>
        public bool CanExecute(object parameter)
        {
            return this.OnCanExecute(parameter);
        }

        /// <summary>
        /// Defines the method to be called when the command is invoked.
        /// </summary>
        /// <param name="parameter">Data used by the command.  If the command does not require data to be passed, this object can be set to null.</param>
        public void Execute(object parameter)
        {
            this.OnExecute(parameter);
        }

        /// <summary>
        /// Called when [can execute].
        /// </summary>
        /// <param name="parameter">The parameter.</param>
        /// <returns></returns>
        protected virtual bool OnCanExecute(object parameter)
        {
            return true;
        }

        /// <summary>
        /// Called when [execute].
        /// </summary>
        /// <param name="parameter">The parameter.</param>
        protected virtual void OnExecute(object parameter)
        {
        }

        /// <summary>
        /// Method used to marshall actions over to the UI thread if they're on a different thread to begin with. Required if the action updates UI controls
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
