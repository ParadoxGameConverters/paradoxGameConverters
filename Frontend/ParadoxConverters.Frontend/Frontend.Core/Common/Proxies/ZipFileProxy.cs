using System.IO.Compression;

namespace Frontend.Core.Common.Proxies
{
    public class ZipFileProxy : IZipFileProxy
    {
        public void ExtractToDirectory(string sourceArchiveFileName, string destinationDirectoryName)
        {
            ZipFile.ExtractToDirectory(sourceArchiveFileName, destinationDirectoryName);
        }

        public ZipArchive Open(string archiveFileName, ZipArchiveMode mode)
        {
            return ZipFile.Open(archiveFileName, mode);
        }

        public void ExtractToFile(ZipArchiveEntry source, string destinationFileName, bool overwrite)
        {
            source.ExtractToFile(destinationFileName, overwrite);
        }
    }
}