using System.Windows.Controls;
using Converter.UI.Settings;

namespace Converter.UI.Framework
{
    public abstract class BaseViewModel : NotifiableBase, IViewModel
    {
        private IView view;

        public BaseViewModel(IView view, string header, ConverterOptions options)
        {
            this.View = view;
            this.Header = header;
            this.Options = options;
        }

        public string Header { get; private set; }

        public ConverterOptions Options { get; private set; }

        public IView View
        {
            get
            {
                return this.view;
            }

            private set
            {
                this.view = value;
                this.view.ViewModel = this;
            }
        }

        public void ActivateTab(object sender, SelectionChangedEventArgs e)
        {
            this.OnTabSelected(sender, e);
        }

        protected virtual void OnTabSelected(object sender, SelectionChangedEventArgs e)
        {
        }
    }
}
