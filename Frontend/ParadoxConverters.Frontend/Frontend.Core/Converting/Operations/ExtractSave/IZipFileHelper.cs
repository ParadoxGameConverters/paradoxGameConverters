namespace Frontend.Core.Converting.Operations.ExtractSave
{
    public interface IZipFileHelper
    {
        void ExtractFile(string sourceArchiveName, string fileToExtract, string destinationDirectoryName);
    }
}