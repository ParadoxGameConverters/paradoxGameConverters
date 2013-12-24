using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Converter.UI.Settings;
using Converter.UI.Helpers;

namespace Converter.UI.Providers
{
    /// <summary>
    /// 
    /// </summary>
    public class ModFilesProvider
    {
        private ConverterOptions options;
        private IList<ModFile> configurationFiles;
        private ModFile modFolder;

        public ModFilesProvider(ConverterOptions options)
        {
            this.options = options;
        }

        public IList<ModFile> ConfigurationFiles
        {
            get
            {
                return this.configurationFiles ?? (this.configurationFiles = this.ReadConfigurationFiles());
            }
        }

        public ModFile ModFolder
        {
            get
            {
                return this.modFolder ?? (this.modFolder = this.ReadModFolder());
            }
        }

        private IList<ModFile> ReadConfigurationFiles()
        {
            var files = new List<ModFile>();
            var configFileName = "Configuration.xml";

            var config = XmlHelper.ReadXmlFile(configFileName);

            if (config == null)
            {
                // Throw new invalid xml exception?
                return null;
            }

            try
            {
                var foundFiles = config.Descendants("converterModFile");

                foreach (var file in foundFiles)
                {
                    var fileName = XElementHelper.ReadStringValue(file, "fileName");
                    var sourcePath = XElementHelper.ReadStringValue(file, "sourcePath");
                    var targetPath = XElementHelper.ReadStringValue(file, "targetPath");

                    files.Add(new ModFile() { FileName = fileName, SourcePath = sourcePath, TargetPath = targetPath });
                }
            }
            catch
            {
            }

            return files;
        }

        private ModFile ReadModFolder()
        {
            ModFile modFolder = new ModFile(); ;

            var configFileName = "Configuration.xml";
            var config = XmlHelper.ReadXmlFile(configFileName);

            if (config == null)
            {
                // Throw new invalid xml exception?
                return null;
            }

            try
            {
                var foundModFolder = config.Descendants("folderName").First();

                modFolder.FileName = XElementHelper.ReadStringValue(foundModFolder, "fileName");
                modFolder.SourcePath = XElementHelper.ReadStringValue(foundModFolder, "sourcePath");
                modFolder.TargetPath = XElementHelper.ReadStringValue(foundModFolder, "targetPath");
            }
            catch
            {
            }

            return modFolder;
        }
    }
}
