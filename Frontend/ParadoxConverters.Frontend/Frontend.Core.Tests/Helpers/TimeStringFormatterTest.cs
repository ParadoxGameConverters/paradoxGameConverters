using System;
using Frontend.Core.Helpers;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Frontend.Core.Tests.Helpers
{
    [TestClass]
    public class TimeStringFormatterTest : UnitTestBase
    {
        [TestMethod]
        public void MinutesSecondTest()
        {
            var expected = "03m 02s";
            var timesspan = new TimeSpan(0, 3, 2);
            var actual = TimeStringFormatter.BuildTimeString(timesspan);
            Assert.AreEqual(expected, actual);
        }

        [TestMethod]
        public void DualDigitMinutesSecondsTest()
        {
            var expected = "12m 24s";
            var timespan = new TimeSpan(0, 12, 24);
            var actual = TimeStringFormatter.BuildTimeString(timespan);
            Assert.AreEqual(expected, actual);
        }
    }
}