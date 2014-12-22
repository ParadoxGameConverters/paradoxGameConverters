using Caliburn.Micro;
using Frontend.Core.Helpers;
using Frontend.Core.Logging;
using Frontend.Core.Model;
using Frontend.Core.Model.Paths;
using Frontend.Core.Model.Paths.Interfaces;
using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace Frontend.Core.Factories.TagReaders
{
    public abstract class TagReaderBase
    {
        private IEventAggregator eventAggregator;

        protected TagReaderBase(IEventAggregator eventAggregator)
        {
            this.eventAggregator = eventAggregator;
        }

        protected IEventAggregator EventAggregator
        {
            get
            {
                return this.eventAggregator;
            }
        }

        protected IList<IAlternativePath> ReadDefaultLocationPaths(XElement xmlElement, string tagName, string friendlyName)
        {
            var alternatives = new List<IAlternativePath>();

            var alternativeTags = XElementHelper.ReadEnumerable(xmlElement, "alternative");

            foreach (var tag in alternativeTags)
            {
                var defaultLocationTypeAsString = XElementHelper.ReadStringValue(tag, "defaultLocationType");

                if (defaultLocationTypeAsString.Equals(RelativeFolderLocationRoot.ConverterFolder.ToString()))
                {
                    alternatives.Add(this.ReadConverterPath(tag, tagName, friendlyName));
                }
                else if (defaultLocationTypeAsString.Equals(RelativeFolderLocationRoot.WindowsUsersFolder.ToString()))
                {
                    alternatives.Add(this.ReadWindowsUserFolderPath(tag, tagName, friendlyName));
                }
                else if (defaultLocationTypeAsString.Equals(RelativeFolderLocationRoot.SteamFolder.ToString()))
                {
                    // If we can find the folder using steam id, do that
                    var steamId = XElementHelper.ReadStringValue(tag, "autoDetectFromSteamId", false);
                    alternatives.Add(this.ReadSteamPath(tag, steamId, tagName, friendlyName));
                }
                else
                {
                    this.EventAggregator.PublishOnUIThread(new LogEntry("Invalid DefaultLocationType. This is either a problem for the developer, or you messed up the .XML file somehow. ",
                        LogEntrySeverity.Error,
                        LogEntrySource.UI));
                }
            }

            return alternatives;
        }

        private IAlternativePath ReadConverterPath(XElement xmlElement, string tagName, string friendlyName)
        {
            var subFolderLocation = XElementHelper.ReadStringValue(xmlElement, "subFolderLocation");
            var absolutePath = Path.Combine(DirectoryHelper.GetFrontendWorkingDirectory(), subFolderLocation);

            return this.BuildAlternativePathObject(absolutePath, tagName, friendlyName);
        }

        private IAlternativePath ReadWindowsUserFolderPath(XElement xmlElement, string tagName, string friendlyName)
        {
            var subFolderLocation = XElementHelper.ReadStringValue(xmlElement, "subFolderLocation");
            var userFolder = DirectoryHelper.GetUsersFolder();
            var absolutePath = Path.Combine(userFolder, subFolderLocation);

            return this.BuildAlternativePathObject(absolutePath, tagName, friendlyName);
        }

        private IAlternativePath ReadSteamPath(XElement xmlElement, string steamId, string tagName, string friendlyName)
        {
            string installationPath = this.ReadSteamFolder(steamId);
            var subFolderLocation = XElementHelper.ReadStringValue(xmlElement, "subFolderLocation", false);
            var absolutePath = Path.Combine(installationPath, subFolderLocation);

            return this.BuildAlternativePathObject(absolutePath, tagName, friendlyName);
        }

        /// <summary>
        /// Attempts to locate the folder where a steam game is installed.
        /// </summary>
        /// <param name="steamId">The steam Id of the game we want to look up. To find the steam id of a game, install the game via steam, 
        /// then create a shortcut on your desktop (from within steam), , show properties for the shortcut, 
        /// and get the number at the end of the URL. Ex: "steam://rungameid/25800"</param>
        /// <returns>The path to where the game is installed</returns>
        private string ReadSteamFolder(string steamId)
        {
            // If installed via Steam, find game installation path.
            // The Easiest way I've found so far is to look at the uninstall keys. 
            if (!string.IsNullOrEmpty(steamId))
            {
                RegistryKey regKey = Registry.LocalMachine.OpenSubKey(@"SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\Steam App " + steamId);

                if (regKey != null)
                {
                    string steamInstallationPath = regKey.GetValue("InstallLocation").ToString();

                    if (!String.IsNullOrEmpty(steamInstallationPath))
                    {
                        if (Directory.Exists(steamInstallationPath))
                        {
                            return steamInstallationPath;
                        }
                    }
                }
            }

            return string.Empty;
        }

        private IAlternativePath BuildAlternativePathObject(string path, string tagName, string friendlyName)
        {
            var alternativePath = new AlternativePath(path, this.Exists(path));

            if (!alternativePath.Exists)
            {
                this.LogExistenceError(alternativePath, tagName, friendlyName);
            }

            return alternativePath;
        }

        private bool Exists(string path)
        {
            // Simplified check whether this is a file or folder. Not really trustworthy, but should be good in most cases.
            bool existsAsPath = Directory.Exists(path);
            bool existsAsFile = File.Exists(path);

            bool exists = (existsAsPath || existsAsFile);

            return exists;
        }

        private void LogExistenceError(IAlternativePath nonExistingAlternativePath, string tagName, string friendlyName)
        {
            var errorMessageOneText = string.Format("Failed to set default value for {0}. This most likely happened because a file or folder was specified in the configuration file, but did not exist on disk.", friendlyName);
            var separatorText = "------";

            this.EventAggregator.PublishOnUIThread(new LogEntry(separatorText, LogEntrySeverity.Warning, LogEntrySource.UI));
            this.EventAggregator.PublishOnUIThread(new LogEntry(errorMessageOneText, LogEntrySeverity.Warning, LogEntrySource.UI));
            this.EventAggregator.PublishOnUIThread(new LogEntry("This may not be a problem depending on which path was missing, and depending on your settings.", LogEntrySeverity.Warning, LogEntrySource.UI));
            this.EventAggregator.PublishOnUIThread(new LogEntry("Should you need to, you can set this file/folder path manually here in the frontend. The expected location was: ", LogEntrySeverity.Warning, LogEntrySource.UI, nonExistingAlternativePath.Path));
        }
    }
}
