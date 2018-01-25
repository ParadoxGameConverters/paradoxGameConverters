using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using Caliburn.Micro;
using Frontend.Core.Common;
using Frontend.Core.Navigation;
using Frontend.Core.ViewModels.Interfaces;

namespace Frontend.Core.ViewModels
{
    /// <summary>
    ///     Base class for the viewmodel conductor.
    /// </summary>
    public abstract class StepConductorBase : ViewModelBase, IStepConductorBase
    {
        private ObservableCollection<IStep> steps;

        protected StepConductorBase(IEventAggregator eventAggregator)
            : base(eventAggregator)
        {
        }

        public ObservableCollection<IStep> Steps
        {
            get { return steps ?? (steps = new ObservableCollection<IStep>()); }
        }

        public IStep CurrentStep { get; private set; }

        public virtual bool CanMoveForward
        {
            get
            {
                if (!CurrentStep.IsValid || Steps.Last() == CurrentStep)
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
                if (Steps.First() == CurrentStep)
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
                    if (CanMoveBackward)
                    {
                        MoveToStep(Steps[Steps.IndexOf(CurrentStep) - 1]);
                    }
                    break;
                case Direction.Forward:
                    MoveToStep(Steps[Steps.IndexOf(CurrentStep) + 1]);
                    break;
            }
        }

        private void CurrentStep_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "IsValid")
            {
                RefreshButtonStates();
            }
        }

        /// <summary>
        ///     Triggers a manual refresh (OnPropertyChange) on the Next and Previous buttons.
        /// </summary>
        private void RefreshButtonStates()
        {
            NotifyOfPropertyChange(() => CanMoveBackward);
            NotifyOfPropertyChange(() => CanMoveForward);
        }

        private void MoveToStep(IStep step)
        {
            if (CurrentStep == step)
            {
                return;
            }

            if (CurrentStep != null)
            {
                CurrentStep.PropertyChanged -= CurrentStep_PropertyChanged;
                CurrentStep.Unload();
            }

            step.Load(null);

            CurrentStep = step;
            CurrentStep.PropertyChanged += CurrentStep_PropertyChanged;
            NotifyOfPropertyChange(() => CurrentStep);
            RefreshButtonStates();
        }
    }
}