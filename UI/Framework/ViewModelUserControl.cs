using System.ComponentModel;
using System.Windows.Controls;

namespace Converter.UI.Framework
{
    public class ViewModelUserControl : UserControl, IView, INotifyPropertyChanged
    {
        private IViewModel viewModel; 
        
        public event PropertyChangedEventHandler PropertyChanged;

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
