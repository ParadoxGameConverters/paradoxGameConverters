using Caliburn.Micro;
using Frontend.Core.Logging;
using Frontend.Core.ViewModels;
using Frontend.Core.ViewModels.Interfaces;
using System.Collections.ObjectModel;
using System.Collections.Specialized;

namespace Frontend.Core.ViewModels
{
    public class LogViewModel : ViewModelBase, ILogViewModel
    {
        private ObservableCollection<LogEntry> logEntries;

        public LogViewModel(IEventAggregator eventAggregator)
            : base(eventAggregator)
        {
        }

        protected override void OnLoading(object parameter)
        {
            this.EventAggregator.Subscribe(this);
        }

        protected override void OnLoaded(object parameter)
        {
            base.OnLoaded(parameter);

            this.LogEntries.CollectionChanged += this.LogEntries_CollectionChanged;
        }

        public ObservableCollection<LogEntry> LogEntries
        {
            get
            {
                return this.logEntries ?? (this.logEntries = new ObservableCollection<LogEntry>());
            }
        }

        public void Handle(LogEntry message)
        {
            this.LogEntries.Add(message);
        }
        
        private void LogEntries_CollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
        }
    }
}
