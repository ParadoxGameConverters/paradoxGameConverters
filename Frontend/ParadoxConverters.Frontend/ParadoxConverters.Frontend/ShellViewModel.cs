using System;
using Caliburn.Micro;
using Frontend.Core.Model;
using Frontend.Core.Navigation;
using Frontend.Core.ViewModels;

namespace Frontend.Client
{
    public class ShellViewModel : IShell, IDisposable
    {
        private readonly IEventAggregator eventAggregator;
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
                if (frameViewModel == null)
                {
                    var options = new ConverterOptions();
                    frameViewModel = new FrameViewModel(eventAggregator);
                    var welcomeViewModel = new WelcomeViewModel(eventAggregator, options);
                    var pathPickerViewModel = new PathPickerViewModel(eventAggregator, options);

                    frameViewModel.Steps.Add(welcomeViewModel);
                    frameViewModel.Steps.Add(pathPickerViewModel);

                    FrameViewModel.Move(Direction.Forward);
                }

                return frameViewModel;
            }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool isDisposing)
        {
            if (isDisposing && !isDisposed)
            {
                frameViewModel.Dispose();
                isDisposed = true;
            }
        }
    }
}