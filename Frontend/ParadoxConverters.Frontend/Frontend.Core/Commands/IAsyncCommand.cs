using System.Threading.Tasks;
using System.Windows.Input;

namespace Frontend.Core.Commands
{
    public interface IAsyncCommand : ICommand
    {
        Task ExecuteAsync(object parameter);
    }
}
