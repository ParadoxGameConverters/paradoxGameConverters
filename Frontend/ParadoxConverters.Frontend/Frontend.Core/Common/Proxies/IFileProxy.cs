using System;
namespace Frontend.Core.Common.Proxies
{
    public interface IFileProxy
    {
        bool Exists(string path);

        void Copy(string sourceFileName, string destFileName, bool overwrite);

        void WriteAllText(string path, string contents);

        string ReadAllText(string path);

        void DeleteFile(string path);
    }
}
