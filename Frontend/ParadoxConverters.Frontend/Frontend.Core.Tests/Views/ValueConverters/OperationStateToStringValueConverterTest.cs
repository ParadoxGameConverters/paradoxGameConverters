using Frontend.Core.Converting.Operations;
using Frontend.Core.Views.ValueConverters;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Frontend.Core.Tests.Views.ValueConverters
{
    [TestClass]
    public class OperationStateToStringValueConverterTest : UnitTestBase
    {
        private OperationStateToStringValueConverter testTarget;

        protected override void OnInitialize()
        {
            base.OnInitialize();
            testTarget = new OperationStateToStringValueConverter();
        }

        protected override void OnCleanup()
        {
            testTarget = null;
            base.OnCleanup();
        }

        [TestMethod]
        public void TestNotStarted()
        {
            var result = testTarget.Convert(OperationState.NotStarted, typeof (OperationState), null, null);

            Assert.AreEqual("Not started", result);
        }

        [TestMethod]
        public void TestInvalidValue()
        {
            var result = testTarget.Convert(null, typeof (OperationState), null, null);

            Assert.AreEqual(string.Empty, result);
        }
    }
}