using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Text;
using System.Windows;
using Frontend.Core.Common.Proxies;
using Frontend.Core.Common.Proxies.Exceptions;

namespace Frontend.Core.Converting.Operations.ExtractSave
{
    public class ZipFileHelper : IZipFileHelper
    {
        private readonly IFileProxy fileProxy;
        private readonly IMessageBoxProxy messageBoxProxy;
        private readonly IZipFileProxy zipFileProxy;

        public ZipFileHelper(IZipFileProxy zipFileProxy, IFileProxy fileProxy, IMessageBoxProxy messageBoxProxy)
        {
            this.zipFileProxy = zipFileProxy;
            this.fileProxy = fileProxy;
            this.messageBoxProxy = messageBoxProxy;
        }

        public void ExtractFile(string sourceArchiveName, string fileToExtract, string destinationDirectoryName)
        {
            //TODO: Wrap in using?
            var zip = zipFileProxy.Open(sourceArchiveName, ZipArchiveMode.Read);
            var zipFileEntry = zip.Entries.FirstOrDefault(entry => entry.FullName.Equals(fileToExtract));

            if (zipFileEntry == null)
            {
                zipFileEntry = zip.Entries.FirstOrDefault(entry => entry.FullName.Equals("game.eu4"));
            }

            if (zipFileEntry == null)
            {
                throw new FileNotFoundInArchiveException();
            }

            var absoluteFileExtractionPath = Path.Combine(destinationDirectoryName, fileToExtract);

            if (fileProxy.Exists(absoluteFileExtractionPath))
            {
                var result = AskForUserPermission(new[] {absoluteFileExtractionPath});

                if (result != MessageBoxResult.Yes)
                {
                    return;
                }
            }

            zipFileProxy.ExtractToFile(zipFileEntry, absoluteFileExtractionPath, true);
        }

        private MessageBoxResult AskForUserPermission(IEnumerable<string> filesThatMustBeDeleted)
        {
            var fileOrFiles = filesThatMustBeDeleted.Count() == 1 ? "file" : "files";

            var sb = new StringBuilder();

            sb.AppendLine(string.Format("The following {0} exists already. Delete before extracting again?", fileOrFiles));
            sb.AppendLine();
            filesThatMustBeDeleted.ForEach(file => sb.AppendLine(file));

            var result = messageBoxProxy.Show(sb.ToString(), string.Format("Delete {0}?", fileOrFiles),
                MessageBoxButton.YesNo);
            return result;
        }
    }
}