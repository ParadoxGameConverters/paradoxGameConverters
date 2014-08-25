using Caliburn.Micro;
using Frontend.Core.Helpers;
using Frontend.Core.Logging;
using Frontend.Core.Model;
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

        protected string ReadDefaultLocationPath(XElement xmlElement)
        {
            var defaultLocationTypeAsString = XElementHelper.ReadStringValue(xmlElement, "defaultLocationType");
            
            if (defaultLocationTypeAsString.Equals(RelativeFolderLocationRoot.ConverterFolder.ToString()))
            {
                return this.ReadConverterPath(xmlElement);
            }
            else if (defaultLocationTypeAsString.Equals(RelativeFolderLocationRoot.WindowsUsersFolder.ToString()))
            {
                return this.ReadWindowsUserFolderPath(xmlElement);
            }
            else if (defaultLocationTypeAsString.Equals(RelativeFolderLocationRoot.SteamFolder.ToString()))
            {
                // If we can find the folder using steam id, do that
                var steamId = XElementHelper.ReadStringValue(xmlElement, "autoDetectFromSteamId", false);
                return this.ReadSteamPath(xmlElement, steamId);
            }
            else
            {
                this.EventAggregator.PublishOnUIThread(new LogEntry("Invalid DefaultLocationType. This is either a problem for the developer, or you messed up the .XML file somehow. ",
                    LogEntrySeverity.Error, 
                    LogEntrySource.UI));
                return string.Empty;
            }
        }

        private string ReadConverterPath(XElement xmlElement)
        {
            var subFolderLocation = XElementHelper.ReadStringValue(xmlElement, "subFolderLocation");
            var absolutePath = Path.Combine(DirectoryHelper.GetFrontendWorkingDirectory(), subFolderLocation);
            return absolutePath;
        }

        private string ReadWindowsUserFolderPath(XElement xmlElement)
        {
            var subFolderLocation = XElementHelper.ReadStringValue(xmlElement, "subFolderLocation");
            var userFolder = DirectoryHelper.GetUsersFolder();
            var absolutePath = Path.Combine(userFolder, subFolderLocation);

            return absolutePath;
        }

        private string ReadSteamPath(XElement xmlElement, string steamId)
        {
            string installationPath = this.ReadSteamFolder(steamId);

            return installationPath;
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
                            eventAggregator.PublishOnUIThread(
                                new LogEntry("Startup: Located Steam game files: ", LogEntrySeverity.Info, LogEntrySource.UI, steamInstallationPath));
                            return steamInstallationPath;
                        }
                    }
                }
            }

            return string.Empty;
        }
    }
}
