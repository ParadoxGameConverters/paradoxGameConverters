using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Common.Proxies
{
    public interface IEnvironmentProxy
    {
        string GetFrontendWorkingDirectory();

        string GetUsersFolder();
    }
}
