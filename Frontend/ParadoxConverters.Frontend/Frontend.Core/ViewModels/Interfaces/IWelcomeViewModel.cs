using Caliburn.Micro;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.Navigation;

namespace Frontend.Core.ViewModels.Interfaces
{
    public interface IWelcomeViewModel : IStep, IHandle<IConverterSettings>
    {
    }
}