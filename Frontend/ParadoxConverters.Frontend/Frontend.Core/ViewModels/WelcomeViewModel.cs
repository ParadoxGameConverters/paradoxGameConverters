using Caliburn.Micro;
using Frontend.Core.Logging;
using Frontend.Core.ViewModels.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.ViewModels
{
    public class WelcomeViewModel : StepViewModelBase, IWelcomeViewModel
    {
        public WelcomeViewModel(IEventAggregator eventAggregator)
            : base(eventAggregator)
        {
        }

        protected override void OnLoading(object parameter)
        {
            if (this.EventAggregator.HandlerExistsFor(typeof(LogEntry)))
            {
                this.EventAggregator.Publish(new LogEntry("Application starting", LogEntrySeverity.Info, LogEntrySource.UI), null);
            }
        }
    }
}
