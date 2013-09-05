using System;
using System.Windows.Input;
using Converter.UI.Settings;
using System.Windows.Threading;

namespace Converter.UI.Commands
{
    public abstract class CommandBase : DispatcherObject, ICommand
    {
        private ConverterOptions options;

        protected CommandBase(ConverterOptions options)
        {
            this.options = options;
        }

        public event EventHandler CanExecuteChanged
        {
            add { CommandManager.RequerySuggested += value; }

            remove { CommandManager.RequerySuggested -= value; }
        }

        protected ConverterOptions Options
        {
            get
            {
                return this.options;
            }
        }

        public bool CanExecute(object parameter)
        {
            return this.OnCanExecute(parameter);
        }

        public void Execute(object parameter)
        {
            this.OnExecute(parameter);
        }

        protected virtual bool OnCanExecute(object parameter)
        {
            return true;
        }

        protected virtual void OnExecute(object parameter)
        {
        }

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
