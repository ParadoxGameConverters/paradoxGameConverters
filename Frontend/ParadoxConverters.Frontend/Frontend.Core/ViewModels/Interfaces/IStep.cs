

using Frontend.Core.Model.Interfaces;
namespace Frontend.Core.ViewModels.Interfaces
{
    public interface IStep : IViewModelBase
    {
        IConverterOptions Options { get; }
    }
}
