using Caliburn.Micro;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.ViewModels
{
    public abstract class ViewModelBase : PropertyChangedBase
    {
        private IEventAggregator eventAggregator;

        public ViewModelBase(IEventAggregator eventAggregator)
        {
            this.eventAggregator = eventAggregator;
        }

        protected IEventAggregator EventAggregator
        {
            get
            {
                return this.eventAggregator;
            }
        }

        public void Load(object parameter)
        {
            this.OnLoading(parameter);
            this.OnLoad(parameter);
            this.OnLoaded(parameter);
        }

        public void Unload()
        {
            this.OnUnloading();
            this.OnUnload();
            this.OnUnloaded();
        }

        protected virtual void OnLoading(object parameter)
        {
        }

        protected virtual void OnLoad(object parameter)
        {
        }

        protected virtual void OnLoaded(object parameter)
        {
        }

        protected virtual void OnUnloading()
        {
        }

        protected virtual void OnUnload()
        {
        }

        protected virtual void OnUnloaded()
        {
        }
    }
}
