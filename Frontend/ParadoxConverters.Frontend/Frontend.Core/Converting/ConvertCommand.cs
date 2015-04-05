using Caliburn.Micro;
using Frontend.Core.Commands;
using Frontend.Core.Converting.Tasks;
using Frontend.Core.Model.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Converting
{
    public class ConvertCommand : CommandBase
    {
        private ITaskProvider taskProvider;

        public ConvertCommand(IEventAggregator eventAggregator, IConverterOptions options, ITaskProvider taskProvider)
            : base(eventAggregator, options)
        {
            this.taskProvider = taskProvider;
        }

        protected override bool OnCanExecute(object parameter)
        {
            return this.taskProvider.Tasks.All(task => task.CanExecute(parameter));
        }

        //public override Task ExecuteAsync(object parameter)
        //{
        //    foreach (var task in this.taskProvider.Tasks)
        //    {
        //        task.Execute(parameter);
        //    }
        //}

        protected async override void OnExecute(object parameter)
        {
            foreach (var task in this.taskProvider.Tasks)
            {
                await task.Execute(parameter);
            }
        }
    }
}
