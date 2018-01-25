using System.IO;
using Frontend.Core.Model.Interfaces;

namespace Frontend.Core.Helpers
{
    /// <summary>
    ///     Helper class that finds working directories, whether for the frontend or the various converters
    /// </summary>
    public class DirectoryHelper : IDirectoryHelper
    {
        public string GetConverterWorkingDirectory(IConverterSettings currentConverter)
        {
            return Path.GetDirectoryName(currentConverter.AbsoluteConverter.SelectedValue);
        }
    }
}