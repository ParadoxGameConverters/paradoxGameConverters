using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Converter.UI.Settings;
using System.IO;
using Converter.UI.Helpers;

namespace Converter.UI.Commands
{
    /// <summary>
    /// Command responsible for installing the converter mod to the users target game mod folder
    /// </summary>
    public class InstallConverterModCommand : CommandBase
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="InstallConverterModCommand"/> class.
        /// </summary>
        /// <param name="options">The options.</param>
        public InstallConverterModCommand(ConverterOptions options)
            : base(options)
        {
        }

        /// <summary>
        /// CanExecute check. Called to verify that the Execute method is allowed to run.
        /// </summary>
        /// <param name="parameter">The parameter.</param>
        /// <returns></returns>
        protected override bool OnCanExecute(object parameter)
        {
            return false;

            //return this.Options.UseConverterMod;
        }

        /// <summary>
        /// Called when [execute].
        /// </summary>
        /// <param name="parameter">The parameter.</param>
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
                DirectoryCopyHelper.DirectoryCopy(absoluteSourcePath, absoluteTargetPath, true, true);
                // Log success
            }
            catch
            {
                // Log failure
            }
        }
    }
}
