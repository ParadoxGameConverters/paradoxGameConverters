using Caliburn.Micro;
using Frontend.Core.Model.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Threading;

namespace Frontend.Core.Converting.Tasks
{
    public abstract class ConverterTaskBase : DispatcherObject, ITask
    {
        protected ConverterTaskBase(IEventAggregator eventAggregator, IConverterOptions options)
        {
            this.EventAggregator = eventAggregator;
        }

        public abstract string FriendlyName { get; }

        protected IEventAggregator EventAggregator { get; private set; }

        protected IConverterOptions Options { get; private set; }

        public async Task Execute(object parameter)
        {
            //await this.BeforeExecute();
            await this.OnExecute(parameter);
            //await this.AfterExecute();
        }

        //protected virtual Task BeforeExecute()
        //{
        //}

        protected abstract Task OnExecute(object parameter);

        //protected virtual Task AfterExecute()
        //{
        //}
        
        /////// <summary>
        /////// Marshalls the method.
        /////// </summary>
        /////// <param name="action">The action.</param>
        /////// <param name="priority">The priority.</param>
        ////protected void MarshallMethod(System.Action action, DispatcherPriority priority)
        ////{
        ////    if (!this.Dispatcher.CheckAccess())
        ////    {
        ////        this.Dispatcher.Invoke(action, priority);
        ////        return;
        ////    }

        ////    action();
        ////}

        public virtual bool CanExecute(object parameter)
        {
            return true;
        }
    }
}
