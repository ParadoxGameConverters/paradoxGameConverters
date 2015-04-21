using System;
using System.Threading.Tasks;
namespace Frontend.Core.Converting.Operations
{
    public interface IOperationViewModel
    {
        string Description { get; }
        void Load(IOperation operation);
        OperationResult Process();
        bool CanRun();
        OperationState State { get; set; }
    }
}
