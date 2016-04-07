namespace Frontend.Core.Model.Paths.Interfaces
{
    public interface IRequiredFile : IRequiredItemBase
    {
        string Extension { get; }
        string PredefinedFileName { get; }
    }
}