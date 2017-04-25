namespace Frontend.Core.Helpers
{
    public interface IDirectoryCopyHelper
    {
        void DirectoryCopy(string sourceDirName, string destDirName, bool copySubDirs, bool overwrite);
    }
}