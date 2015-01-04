

namespace Frontend.Core.ViewModels
{
    using Caliburn.Micro;
    using Frontend.Core.ViewModels.Interfaces;
    using System.Collections.ObjectModel;
    using System.ComponentModel;
    using System.Linq;

    /// <summary>
    /// Base class for the viewmodel conductor. 
    /// </summary>
    public abstract class StepConductorBase : ViewModelBase, IStepConductorBase
    {
        private ObservableCollection<IStep> steps;
        private IStep currentStep;

        public StepConductorBase(IEventAggregator eventAggregator)
            : base(eventAggregator)
        {
        }

        public ObservableCollection<IStep> Steps
        {
            get
            {
                return this.steps ?? (this.steps = new ObservableCollection<IStep>());
            }
        }

        public IStep CurrentStep
        {
            get
            {
                return this.currentStep;
            }
        }

        public virtual bool CanMoveForward
        {
            get
            {
                if (!this.CurrentStep.IsValid || this.Steps.Last() == this.CurrentStep)
                {
                    return false;
                }

                return true;
            }
        }

        public virtual bool CanMoveBackward
        {
            get
            {
                if (this.Steps.First() == this.CurrentStep)
                {
                    return false;
                }

                return true;
            }
        }

        public void Move(Direction direction)
        {
            switch (direction)
            {
                case Direction.Backward:
                    if (this.CanMoveBackward)
                    {
                        this.MoveToStep(this.Steps[this.Steps.IndexOf(this.CurrentStep) - 1]);
                    }
                    break;
                case Direction.Forward:
                    this.MoveToStep(this.Steps[this.Steps.IndexOf(this.CurrentStep) + 1]);
                    break;
            }
        }

        private void CurrentStep_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "IsValid")
            {
                this.RefreshButtonStates();
            }
        }

        /// <summary>
        /// Triggers a manual refresh (OnPropertyChange) on the Next and Previous buttons.
        /// </summary>
        private void RefreshButtonStates()
        {
            this.NotifyOfPropertyChange(() => this.CanMoveBackward);
            this.NotifyOfPropertyChange(() => this.CanMoveForward);
        }

        private void MoveToStep(IStep step)
        {
            if (this.currentStep == step)
            {
                return;
            }

            if (this.CurrentStep != null)
            {
                this.CurrentStep.PropertyChanged -= this.CurrentStep_PropertyChanged;
                this.CurrentStep.Unload();
            }

            step.Load(null);

            this.currentStep = step;
            this.CurrentStep.PropertyChanged += this.CurrentStep_PropertyChanged;
            this.NotifyOfPropertyChange(() => this.CurrentStep);
            this.RefreshButtonStates();
        }
    }
}
