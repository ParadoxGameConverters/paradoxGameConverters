using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Frontend.Core.Tests
{
    public class UnitTestBase
    {
        public TestContext TestContext { get; set; }

        [TestInitialize]
        public void Initialize()
        {
            OnInitialize();
        }

        [TestCleanup]
        public void Cleanup()
        {
            OnCleanup();
        }

        protected virtual void OnInitialize()
        {
        }

        protected virtual void OnCleanup()
        {
        }
    }
}