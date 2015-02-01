using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Frontend.Core.Tests
{
    public class UnitTestBase
    {
        public TestContext TestContext
        {
            get;
            set;
        }

        [TestInitialize]
        public void Initialize()
        {
            this.OnInitialize();
        }

        [TestCleanup]
        public void Cleanup()
        {
            this.OnCleanup();
        }

        protected virtual void OnInitialize(){}

        protected virtual void OnCleanup() { }
    }
}
