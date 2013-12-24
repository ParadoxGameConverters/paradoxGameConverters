using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Security.AccessControl;
using System.Security.Principal;

namespace Converter.UI.Helpers
{
    /// <summary>
    /// A helper class that handles various disk permission tasks
    /// </summary>
    public static class DiskPermissionHelper
    {
        /// <summary>
        /// Determines whether this instance [can write file automatic folder] the specified path.
        /// </summary>
        /// <param name="path">The path.</param>
        /// <returns></returns>
        public static bool CanWriteFileToFolder(string path)
        {
            bool canWrite = false;

            string NtAccountName = @"MyDomain\MyUserOrGroup";

            DirectoryInfo di = new DirectoryInfo(path);
            DirectorySecurity acl = di.GetAccessControl(AccessControlSections.All);
            AuthorizationRuleCollection rules = acl.GetAccessRules(true, true, typeof(NTAccount));

            //Go through the rules returned from the DirectorySecurity
            foreach (AuthorizationRule rule in rules)
            {
                //If we find one that matches the identity we are looking for
                if (rule.IdentityReference.Value.Equals(NtAccountName, StringComparison.CurrentCultureIgnoreCase))
                {
                    //Cast to a FileSystemAccessRule to check for access rights
                    if ((((FileSystemAccessRule)rule).FileSystemRights & FileSystemRights.WriteData) > 0)
                    {
                        canWrite = true;
                    }
                    else
                    {
                        canWrite = false;
                    }
                }
            }

            return canWrite;
        }
    }
}
