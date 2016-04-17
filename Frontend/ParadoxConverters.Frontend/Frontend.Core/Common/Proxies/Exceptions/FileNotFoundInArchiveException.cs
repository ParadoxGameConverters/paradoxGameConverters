using System;

namespace Frontend.Core.Common.Proxies.Exceptions
{
    public class FileNotFoundInArchiveException : ArgumentException
    {
        public FileNotFoundInArchiveException()
            : base("The given file name was not found in the archive. Did you rename the save game?")
        {
        }
    }
}