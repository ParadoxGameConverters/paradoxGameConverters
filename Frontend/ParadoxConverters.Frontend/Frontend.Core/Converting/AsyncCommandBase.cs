using System;
using System.Threading.Tasks;
using System.Windows.Input;

namespace Frontend.Core.Converting
{
    public abstract class AsyncCommandBase : ICommand
    {
        public abstract bool CanExecute(object parameter);

        public async void Execute(object parameter)
        {
            await ExecuteAsync(parameter);
        }

        public event EventHandler CanExecuteChanged
        {
            add { CommandManager.RequerySuggested += value; }
            remove { CommandManager.RequerySuggested -= value; }
        }

        public abstract Task ExecuteAsync(object parameter);

        protected void RaiseCanExecuteChanged()
        {
            CommandManager.InvalidateRequerySuggested();
        }
    }
}