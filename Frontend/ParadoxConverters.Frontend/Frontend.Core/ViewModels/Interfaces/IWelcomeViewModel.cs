using Caliburn.Micro;
using Frontend.Core.Model.Interfaces;

namespace Frontend.Core.ViewModels.Interfaces
{
    public interface IWelcomeViewModel : IStep, IHandle<IConverterSettings>
    {
    }
}
