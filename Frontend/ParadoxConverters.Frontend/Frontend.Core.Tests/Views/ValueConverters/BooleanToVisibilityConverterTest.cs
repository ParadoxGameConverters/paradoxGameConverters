using System.Windows;
using Frontend.Core.Views.ValueConverters;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Frontend.Core.Tests.Views.ValueConverters
{
    [TestClass]
    public class BooleanToVisibilityConverterTest : UnitTestBase
    {
        private BooleanToVisibilityConverter testTarget;

        protected override void OnInitialize()
        {
            base.OnInitialize();
            testTarget = new BooleanToVisibilityConverter();
        }

        protected override void OnCleanup()
        {
            testTarget = null;
            base.OnCleanup();
        }

        [TestMethod]
        public void TestConvertTrueWhenVisibleWhenIsTrue()
        {
            var visibility = testTarget.Convert(true, typeof (bool), VisibleWhen.True, null);
            Assert.AreEqual(Visibility.Visible, visibility);
        }

        [TestMethod]
        public void TestConvertTrueWhenVisibleWhenIsFalse()
        {
            var visibility = testTarget.Convert(true, typeof (bool), VisibleWhen.False, null);
            Assert.AreEqual(Visibility.Collapsed, visibility);
        }

        [TestMethod]
        public void TestConvertFalseWhenVisibleWhenIsTrue()
        {
            var visibility = testTarget.Convert(false, typeof (bool), VisibleWhen.True, null);
            Assert.AreEqual(Visibility.Collapsed, visibility);
        }

        [TestMethod]
        public void TestConvertFalseWhenVisibleWhenIsFalse()
        {
            var visibility = testTarget.Convert(false, typeof (bool), VisibleWhen.False, null);
            Assert.AreEqual(Visibility.Visible, visibility);
        }
    }
}