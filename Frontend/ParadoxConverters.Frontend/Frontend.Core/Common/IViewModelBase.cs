using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Common
{
    public interface IViewModelBase : INotifyPropertyChanged
    {
        void Load(object parameter);
        void Unload();
    }
}
