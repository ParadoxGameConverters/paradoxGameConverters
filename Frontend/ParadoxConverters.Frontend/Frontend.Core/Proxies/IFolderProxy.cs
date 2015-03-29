using System;
namespace Frontend.Core.Proxies
{
    public interface IFolderProxy
    {
        string GetFileNameWithoutExtension(string path);

        string Combine(string path1, string path2, string path3);

        string Combine(string path1, string path2);

        bool Exists(string path);
    }
}
