using System.ComponentModel;
using System.Windows.Controls;

namespace Converter.UI.Framework
{
    /// <summary>
    /// Base class for all Views, mainly used to avoid more code in the code behind files than strictly necessary.
    /// </summary>
    public class ViewModelUserControl : UserControl, IView, INotifyPropertyChanged
    {
        /// <summary>
        /// The view model
        /// </summary>
        private IViewModel viewModel;

        /// <summary>
        /// Occurs when a property value changes.
        /// </summary>
        public event PropertyChangedEventHandler PropertyChanged;

        /// <summary>
        /// Gets or sets the view model.
        /// </summary>
        /// <value>
        /// The view model.
        /// </value>
        public IViewModel ViewModel
        {
            get
            {
                return this.viewModel;
            }

            set
            {
                if (this.viewModel == value)
                {
                    return;
                }

                this.viewModel = value;
                this.DataContext = this.viewModel;
                this.RaisePropertyChanged("ViewModel");
            }
        }

        /// <summary>
        /// Raises the property changed.
        /// </summary>
        /// <param name="propertyName">Name of the property.</param>
        protected void RaisePropertyChanged(string propertyName)
        {
            PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
            {
                handler(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}
