using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Converter.UI.Settings;
using System.IO;
using Converter.UI.Helpers;

namespace Converter.UI.Commands
{
    public class InstallConverterModCommand : CommandBase
    {
        public InstallConverterModCommand(ConverterOptions options)
            : base(options)
        {
        }

        protected override bool OnCanExecute(object parameter)
        {
            return this.Options.UseConverterMod;
        }

        protected override void OnExecute(object parameter)
        {
            var modFolder = this.Options.ModFilesProvider.ModFolder;

            var absoluteSourcePath = Path.Combine(Environment.CurrentDirectory, modFolder.SourcePath);

            // Verify source data
            if (!File.Exists(absoluteSourcePath))
            {
                // Log error
                return;
            }

            var absoluteTargetPath = Path.Combine(this.Options.TargetGame.InstallationPath, modFolder.TargetPath);

            // Overwrite needed?
            if (!File.Exists(absoluteTargetPath))
            {
                // Ask permission for overwrite (or delete whole thing, then recopy?)
            }

            try
            {
                DirectoryCopyHelper.DirectoryCopy(absoluteSourcePath, absoluteTargetPath, true);
                // Log success
            }
            catch
            {
                // Log failure
            }
        }
    }
}
