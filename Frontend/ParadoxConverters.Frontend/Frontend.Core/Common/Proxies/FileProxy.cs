using System.Collections.Generic;
using System.IO;

namespace Frontend.Core.Common.Proxies
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

        public void WriteAllText(string path, string contents)
        {
            File.WriteAllText(path, contents);
        }


        public string ReadAllText(string path)
        {
            return File.ReadAllText(path);
        }

        public void DeleteFile(string path)
        {
            File.Delete(path);
        }


        public IEnumerable<string> ExistsMany(IEnumerable<string> paths)
        {
            var result = new List<string>();

            foreach(var path in paths)
            {
                if (this.Exists(path))
                {
                    result.Add(path);
                }
            }

            return result;
        }
    }
}
