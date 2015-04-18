using System.IO;

namespace Frontend.Core.Common.Proxies
{
    public class FolderProxy : IFolderProxy
    {
        public string GetFileNameWithoutExtension(string path)
        {
            return Path.GetFileNameWithoutExtension(path);
        }

        public string Combine(string path1, string path2, string path3)
        {
            return Path.Combine(path1, path2, path3);
        }

        public string Combine(string path1, string path2)
        {
            return Path.Combine(path1, path2);
        }

        public bool Exists(string path)
        {
            return Directory.Exists(path);
        }
    }
}
