using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Common.Proxies
{
    public class EnvironmentProxy : IEnvironmentProxy
    {
        public string GetFrontendWorkingDirectory()
        {
            return Environment.CurrentDirectory;
        }

        public string GetUsersFolder()
        {
            return Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
        }
    }
}
