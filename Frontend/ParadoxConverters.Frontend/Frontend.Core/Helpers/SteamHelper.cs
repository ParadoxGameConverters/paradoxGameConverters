using Caliburn.Micro;
using Frontend.Core.Logging;
using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Helpers
{
    public static class SteamHelper
    {
        /// <summary>
        /// Attempts to locate the folder where a steam game is installed.
        /// There might be cleaner ways of doing this, but this seems to do the job for EU3 at least.
        /// </summary>
        /// <param name="steamId">The steam Id of the game we want to look up. To find the steam id of a game, install the game via steam, 
        /// then create a shortcut on your desktop (from within steam), , show properties for the shortcut, 
        /// and get the number at the end of the URL. Ex: "steam://rungameid/25800"</param>
        /// <returns>The path to where the game is installed</returns>
        public static string GetSteamInstallationFolder(IEventAggregator eventAggregator, string steamId)
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
