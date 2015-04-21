namespace Frontend.Core.Converting.Operations
{
    public enum OperationState
    {
        NotStarted,

        InProgress,

        Success,

        CompleteWithWarnings,

        CompleteWithErrors,

        Cancelled,

        Invalid
    }
}