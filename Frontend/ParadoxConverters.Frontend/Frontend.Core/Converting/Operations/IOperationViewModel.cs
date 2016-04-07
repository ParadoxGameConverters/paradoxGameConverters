namespace Frontend.Core.Converting.Operations
{
    public interface IOperationViewModel
    {
        string Description { get; }
        OperationState State { get; set; }
        void Load(IOperation operation);
        OperationResult Process();
        bool CanRun();
    }
}