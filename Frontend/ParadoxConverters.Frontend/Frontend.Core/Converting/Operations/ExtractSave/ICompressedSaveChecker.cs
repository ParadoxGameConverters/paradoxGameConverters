using System;
namespace Frontend.Core.Converting.Operations.ExtractSave
{
    public interface ICompressedSaveChecker
    {
        bool CheckIfCompressedSave(string path);
    }
}
