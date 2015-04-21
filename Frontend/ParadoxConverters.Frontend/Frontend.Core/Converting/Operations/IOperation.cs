namespace Frontend.Core.Converting.Operations
{
    public interface IOperation
    {
        string Description { get; }
        OperationResult Process();
        bool CanRun();
    }
}