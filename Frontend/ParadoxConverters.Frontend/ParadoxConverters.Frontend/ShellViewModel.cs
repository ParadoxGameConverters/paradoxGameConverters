

namespace Frontend.Client
{
    using Caliburn.Micro;
    using Frontend.Core.Model;
    using Frontend.Core.Navigation;
    using Frontend.Core.ViewModels;
    using System;

    public class ShellViewModel : IShell , IDisposable
    {
        private IEventAggregator eventAggregator;
        private FrameViewModel frameViewModel;
        private bool isDisposed;

        public ShellViewModel(IEventAggregator eventAggregator)
        {
            this.eventAggregator = eventAggregator;
        }

        public FrameViewModel FrameViewModel 
        { 
            get
            {
                if (this.frameViewModel == null)
                {
                    var options = new ConverterOptions();
                    this.frameViewModel = new FrameViewModel(this.eventAggregator);
                    var welcomeViewModel = new WelcomeViewModel(this.eventAggregator, options);
                    var pathPickerViewModel = new PathPickerViewModel(this.eventAggregator, options);

                    this.frameViewModel.Steps.Add(welcomeViewModel);
                    this.frameViewModel.Steps.Add(pathPickerViewModel);

                    this.FrameViewModel.Move(Direction.Forward);
                }

                return this.frameViewModel;
            }
        }

        public void Dispose()
        {
            this.Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool isDisposing)
        {
            if (isDisposing && !this.isDisposed)
            {
                this.frameViewModel.Dispose();
                this.isDisposed = true;
            }
        }
    }
}