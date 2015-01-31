using System;

namespace Frontend.Core.Helpers
{
    public static class TimeStringFormatter
    {
        public static string BuildTimeString(TimeSpan timeSpan)
        {
            return string.Format("{0:D2}m {1:D2}s", timeSpan.Minutes, timeSpan.Seconds);
        }
    }
}
