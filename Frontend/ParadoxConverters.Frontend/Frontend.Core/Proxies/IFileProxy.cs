using System;
namespace Frontend.Core.Proxies
{
    public interface IFileProxy
    {
        bool Exists(string path);

        void Copy(string sourceFileName, string destFileName, bool overwrite);
    }
}
