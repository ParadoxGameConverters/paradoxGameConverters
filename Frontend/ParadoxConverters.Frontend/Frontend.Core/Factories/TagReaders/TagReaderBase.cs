using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Xml.Linq;
using Caliburn.Micro;
using Frontend.Core.Common.Proxies;
using Frontend.Core.Helpers;
using Frontend.Core.Logging;
using Frontend.Core.Model;
using Frontend.Core.Model.Paths;
using Frontend.Core.Model.Paths.Interfaces;
using Microsoft.Win32;

namespace Frontend.Core.Factories.TagReaders
{
    public abstract class TagReaderBase
    {
        private readonly IEnvironmentProxy environmentProxy;
        private IDirectoryHelper directoryHelper;

        protected TagReaderBase(IEventAggregator eventAggregator, IDirectoryHelper directoryHelper,
            IEnvironmentProxy environmentProxy)
        {
            EventAggregator = eventAggregator;
            this.directoryHelper = directoryHelper;
            this.environmentProxy = environmentProxy;
        }

        protected IEventAggregator EventAggregator { get; private set; }

        protected IList<IAlternativePath> ReadDefaultLocationPaths(XElement xmlElement, string tagName,
            string friendlyName, string predefinedFileName = null)
        {
            var alternatives = new List<IAlternativePath>();

            var alternativeTags = XElementHelper.ReadEnumerable(xmlElement, "alternative");

            foreach (var tag in alternativeTags)
            {
                var defaultLocationTypeAsString = XElementHelper.ReadStringValue(tag, "defaultLocationType");

                if (defaultLocationTypeAsString.Equals(RelativeFolderLocationRoot.ConverterFolder.ToString()))
                {
                    alternatives.Add(ReadConverterPath(tag, tagName, friendlyName, predefinedFileName));
                }
                else if (defaultLocationTypeAsString.Equals(RelativeFolderLocationRoot.WindowsUsersFolder.ToString()))
                {
                    alternatives.Add(ReadWindowsUserFolderPath(tag, tagName, friendlyName));
                }
                else if (defaultLocationTypeAsString.Equals(RelativeFolderLocationRoot.SteamFolder.ToString()))
                {
                    // If we can find the folder using steam id, do that
                    var steamId = XElementHelper.ReadStringValue(tag, "autoDetectFromSteamId", false);
                    alternatives.Add(ReadSteamPath(tag, steamId, tagName, friendlyName));
                }
                else
                {
                    EventAggregator.PublishOnUIThread(
                        new LogEntry(
                            "Invalid DefaultLocationType. This is either a problem for the developer, or you messed up the .XML file somehow. ",
                            LogEntrySeverity.Error,
                            LogEntrySource.UI));
                }
            }

            // Hack: need some way to make alternative paths aware that their required file/folder is hidden/not mandatory, in which case
            // this check/logging isn't needed
            if (alternatives.All(a => !a.Exists) && !tagName.Equals("faq"))
            {
                alternatives.ForEach(a => this.LogExistenceError(a, tagName, friendlyName));
            }

            return alternatives;
        }

        private IAlternativePath ReadConverterPath(XElement xmlElement, string tagName, string friendlyName, string predefinedFileName)
        {
            var subFolderLocation = XElementHelper.ReadStringValue(xmlElement, "subFolderLocation");

            // If a predefined file name is set, include it in the absolute path (and hence the alternativepath.exists check)
            var absolutePath = string.IsNullOrEmpty(predefinedFileName) ? 
                Path.Combine(environmentProxy.GetFrontendWorkingDirectory(), subFolderLocation) :
                Path.Combine(environmentProxy.GetFrontendWorkingDirectory(), subFolderLocation, predefinedFileName);

            return BuildAlternativePathObject(absolutePath, tagName, friendlyName);
        }

        private IAlternativePath ReadWindowsUserFolderPath(XElement xmlElement, string tagName, string friendlyName)
        {
            var subFolderLocation = XElementHelper.ReadStringValue(xmlElement, "subFolderLocation");
            var userFolder = environmentProxy.GetUsersFolder();
            var absolutePath = Path.Combine(userFolder, subFolderLocation);

            return BuildAlternativePathObject(absolutePath, tagName, friendlyName);
        }

        private IAlternativePath ReadSteamPath(XElement xmlElement, string steamId, string tagName, string friendlyName)
        {
            var installationPath = ReadSteamFolder(steamId);
            var subFolderLocation = XElementHelper.ReadStringValue(xmlElement, "subFolderLocation", false);
            var absolutePath = Path.Combine(installationPath, subFolderLocation);

            return BuildAlternativePathObject(absolutePath, tagName, friendlyName);
        }

        /// <summary>
        ///     Attempts to locate the folder where a steam game is installed.
        /// </summary>
        /// <param name="steamId">
        ///     The steam Id of the game we want to look up. To find the steam id of a game, install the game via steam,
        ///     then create a shortcut on your desktop (from within steam), , show properties for the shortcut,
        ///     and get the number at the end of the URL. Ex: "steam://rungameid/25800"
        /// </param>
        /// <returns>The path to where the game is installed</returns>
        private string ReadSteamFolder(string steamId)
        {
            // If installed via Steam, find game installation path.
            // The Easiest way I've found so far is to look at the uninstall keys. 
            if (!string.IsNullOrEmpty(steamId))
            {
                var regKey =
                    Registry.LocalMachine.OpenSubKey(
                        @"SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\Steam App " + steamId);

                if (regKey != null)
                {
                    var steamInstallationPath = regKey.GetValue("InstallLocation").ToString();

                    if (!string.IsNullOrEmpty(steamInstallationPath))
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
            var alternativePath = new AlternativePath(path, Exists(path));

            return alternativePath;
        }

        private bool Exists(string path)
        {
            // Simplified check whether this is a file or folder. Not really trustworthy, but should be good in most cases.
            var existsAsPath = Directory.Exists(path);
            var existsAsFile = File.Exists(path);

            var exists = (existsAsPath || existsAsFile);

            return exists;
        }

        private void LogExistenceError(IAlternativePath nonExistingAlternativePath, string tagName, string friendlyName)
        {
            var errorMessageText =
                string.Format(
                    "The path to {0} could not be detected, and need to be set manually here in the frontend. Was expecting it to exist at:",
                    friendlyName);

            EventAggregator.PublishOnUIThread(new LogEntry(errorMessageText, LogEntrySeverity.Info, LogEntrySource.UI,
                nonExistingAlternativePath.Path));
        }
    }
}