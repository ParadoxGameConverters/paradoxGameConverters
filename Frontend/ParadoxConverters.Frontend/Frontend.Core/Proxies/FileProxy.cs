using System.IO;

namespace Frontend.Core.Proxies
{
    public class FileProxy : IFileProxy
    {
        public bool Exists(string path)
        {
            return File.Exists(path);
        }

        public void Copy(string sourceFileName, string destFileName, bool overwrite)
        {
            File.Copy(sourceFileName, destFileName, overwrite);
        }
    }
}
