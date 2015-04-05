using System.Threading.Tasks;

namespace Frontend.Core.Converting.Operations
{
    public interface IOperation
    {
        string Description { get; }

        Task<OperationResult> Process();

        bool CanRun();
    }
}
