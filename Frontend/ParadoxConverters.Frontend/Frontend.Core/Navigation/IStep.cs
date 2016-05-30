using Frontend.Core.Common;
using Frontend.Core.Model.Interfaces;

namespace Frontend.Core.Navigation
{
    public interface IStep : IViewModelBase
    {
        IConverterOptions Options { get; }
        bool IsValid { get; }
    }
}