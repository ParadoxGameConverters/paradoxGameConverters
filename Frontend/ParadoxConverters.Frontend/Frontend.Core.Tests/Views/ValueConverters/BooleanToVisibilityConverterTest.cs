using Frontend.Core.Views.ValueConverters;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace Frontend.Core.Tests.Views.ValueConverters
{
    [TestClass]
    public class BooleanToVisibilityConverterTest : UnitTestBase
    {
        private BooleanToVisibilityConverter testTarget;

        protected override void OnInitialize()
        {
            base.OnInitialize();
            this.testTarget = new BooleanToVisibilityConverter();
        }

        protected override void OnCleanup()
        {
            this.testTarget = null;
            base.OnCleanup();
        }

        [TestMethod]
        public void TestConvertTrueWhenVisibleWhenIsTrue()
        {
            var visibility = this.testTarget.Convert(true, typeof(bool), VisibleWhen.True, null);
            Assert.AreEqual(Visibility.Visible, visibility);
        }

        [TestMethod]
        public void TestConvertTrueWhenVisibleWhenIsFalse()
        {
            var visibility = this.testTarget.Convert(true, typeof(bool), VisibleWhen.False, null);
            Assert.AreEqual(Visibility.Collapsed, visibility);
        }

        [TestMethod]
        public void TestConvertFalseWhenVisibleWhenIsTrue()
        {
            var visibility = this.testTarget.Convert(false, typeof(bool), VisibleWhen.True, null);
            Assert.AreEqual(Visibility.Collapsed, visibility);
        }

        [TestMethod]
        public void TestConvertFalseWhenVisibleWhenIsFalse()
        {
            var visibility = this.testTarget.Convert(false, typeof(bool), VisibleWhen.False, null);
            Assert.AreEqual(Visibility.Visible, visibility);
        }
    }
}
