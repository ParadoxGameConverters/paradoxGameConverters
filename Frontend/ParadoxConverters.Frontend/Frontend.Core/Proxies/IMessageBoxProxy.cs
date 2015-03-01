using System.Windows;

namespace Frontend.Core.Proxies
{
    public interface IMessageBoxProxy
    {
        MessageBoxResult Show(string messageBoxText, string caption, MessageBoxButton button);
    }
}
