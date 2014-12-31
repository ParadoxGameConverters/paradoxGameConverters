using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.ViewModels.Interfaces
{
    public interface IViewModelBase
    {
        void Load(object parameter);
        void Unload();
    }
}
