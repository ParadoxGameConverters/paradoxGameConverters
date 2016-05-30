using System.ComponentModel;

namespace Frontend.Core.Common
{
    public interface IViewModelBase : INotifyPropertyChanged
    {
        void Load(object parameter);
        void Unload();
    }
}