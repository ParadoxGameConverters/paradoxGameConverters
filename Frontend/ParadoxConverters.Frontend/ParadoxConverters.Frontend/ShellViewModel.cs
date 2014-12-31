

namespace Frontend.Client
{
    using Caliburn.Micro;
    using Frontend.Core.Logging;
    using Frontend.Core.Model;
    using Frontend.Core.ViewModels;
    using Frontend.Core.ViewModels.Interfaces;

    public class ShellViewModel : IShell 
    {
        private IEventAggregator eventAggregator;
        private IFrameViewModel frameViewModel;

        public ShellViewModel(IEventAggregator eventAggregator)
        {
            this.eventAggregator = eventAggregator;
        }

        public IFrameViewModel FrameViewModel 
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
    }
}