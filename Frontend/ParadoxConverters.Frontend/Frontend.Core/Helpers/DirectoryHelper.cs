using Frontend.Core.Model.Interfaces;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Helpers
{
    /// <summary>
    /// Helper class that finds working directories, whether for the frontend or the various converters
    /// </summary>
    public static class DirectoryHelper
    {
        public static string GetFrontendWorkingDirectory()
        {
            return Environment.CurrentDirectory;
        }

        public static string GetConverterWorkingDirectory(IConverterSettings currentConverter)
        {
            return Path.GetDirectoryName(currentConverter.AbsoluteConverter.SelectedValue);
        }

        public static string GetUsersFolder()
        {
            return Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
        }
    }
}
