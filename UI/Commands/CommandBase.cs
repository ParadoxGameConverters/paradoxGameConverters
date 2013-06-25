using System;
using System.Windows.Input;
using Converter.UI.Settings;

namespace Converter.UI.Commands
{
    public abstract class CommandBase : ICommand
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
    }
}
