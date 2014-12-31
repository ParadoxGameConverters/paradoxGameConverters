using Frontend.Core.Model.Paths.Interfaces;

namespace Frontend.Core.Model.Paths
{
    public class AlternativePath : IAlternativePath
    {
        public AlternativePath(string path, bool exists)
        {
            this.Path = path;
            this.Exists = exists;
        }

        public string Path { get; set; }
        public bool Exists { get; private set; }
    }
}
