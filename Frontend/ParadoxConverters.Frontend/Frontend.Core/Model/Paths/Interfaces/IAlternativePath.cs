namespace Frontend.Core.Model.Paths.Interfaces
{
    public interface IAlternativePath
    {
        string Path { get; set; }
        bool Exists { get; }
    }
}