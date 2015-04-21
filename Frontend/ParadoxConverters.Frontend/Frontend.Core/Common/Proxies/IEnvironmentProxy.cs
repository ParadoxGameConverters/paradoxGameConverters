namespace Frontend.Core.Common.Proxies
{
    public interface IEnvironmentProxy
    {
        string GetFrontendWorkingDirectory();
        string GetUsersFolder();
    }
}