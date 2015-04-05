using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Converting.Tasks
{
    public interface ITask
    {
        bool CanExecute(object parameter);
        Task Execute(object parameter);
    }
}
