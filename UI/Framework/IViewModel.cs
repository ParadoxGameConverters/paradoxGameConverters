using System.Windows.Controls;

namespace Converter.UI.Framework
{
    public interface IViewModel
    {
        IView View { get; }

        void ActivateTab(object sender, SelectionChangedEventArgs e);
    }
}
