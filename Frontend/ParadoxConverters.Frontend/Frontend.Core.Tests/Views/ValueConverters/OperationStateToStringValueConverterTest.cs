using Frontend.Core.Converting.Operations;
using Frontend.Core.Views.ValueConverters;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Tests.Views.ValueConverters
{
    [TestClass]
    public class OperationStateToStringValueConverterTest : UnitTestBase
    {
        private OperationStateToStringValueConverter testTarget;

        protected override void OnInitialize()
        {
            base.OnInitialize();
            this.testTarget = new OperationStateToStringValueConverter();
        }

        protected override void OnCleanup()
        {
            this.testTarget = null;
            base.OnCleanup();
        }

        [TestMethod]
        public void TestNotStarted()
        {
            var result = this.testTarget.Convert(OperationState.NotStarted, typeof(OperationState), null, null);

            Assert.AreEqual("Not started", result);
        }

        [TestMethod]
        public void TestInvalidValue()
        {
            var result = this.testTarget.Convert(null, typeof(OperationState), null, null);

            Assert.AreEqual(string.Empty, result);
        }
    }
}
