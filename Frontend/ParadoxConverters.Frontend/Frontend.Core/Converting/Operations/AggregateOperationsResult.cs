namespace Frontend.Core.Converting.Operations
{
    public enum AggregateOperationsResult
    {
        NotStarted,

        InProgress,

        Canceled,

        CompletedSuccessfully,

        CompletedWithWarnings,

        CompletedWithErrors
    }
}