namespace Frontend.Core.Converting.Operations.ExtractSave
{
    public interface ICompressedSaveChecker
    {
        bool IsCompressedSave(string path);
    }
}