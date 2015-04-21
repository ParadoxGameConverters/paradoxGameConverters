using Frontend.Core.Common.Proxies;
using Frontend.Core.Converting.Operations.ExtractSave;
using Frontend.Core.Helpers;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.Model.Paths.Interfaces;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;

namespace Frontend.Core.Tests.Converter.Operations.ExtractSave
{
    [TestClass]
    public class ExtractSaveOperationTest : UnitTestBase
    {
        private Mock<IFileProxy> fileProxy;
        private Mock<ICompressedSaveChecker> mockCompressedSaveChecker;
        private Mock<IConverterOptions> mockConverterOptions;
        private Mock<IConverterSettings> mockCurrentConverter;
        private Mock<IDirectoryHelper> mockDirectoryHelper;
        private Mock<IEnvironmentProxy> mockEnvironment;
        private Mock<IMessageBoxProxy> mockMessageBoxProxy;
        private Mock<IRequiredFile> mockSavegame;
        private ExtractSaveOperation testTarget;
        private Mock<IZipFileHelper> zipFileHelper;

        protected override void OnCleanup()
        {
            mockSavegame = null;
            mockCurrentConverter = null;
            testTarget = null;
            mockMessageBoxProxy = null;
            mockDirectoryHelper = null;
            mockConverterOptions = null;
            mockCompressedSaveChecker = null;
            fileProxy = null;
            zipFileHelper = null;
            mockEnvironment = null;
            base.OnCleanup();
        }

        protected override void OnInitialize()
        {
            base.OnInitialize();
            mockCompressedSaveChecker = new Mock<ICompressedSaveChecker>();
            mockConverterOptions = new Mock<IConverterOptions>();
            mockDirectoryHelper = new Mock<IDirectoryHelper>();
            mockMessageBoxProxy = new Mock<IMessageBoxProxy>();
            fileProxy = new Mock<IFileProxy>();
            zipFileHelper = new Mock<IZipFileHelper>();
            mockEnvironment = new Mock<IEnvironmentProxy>();

            mockCurrentConverter = new Mock<IConverterSettings>();
            mockConverterOptions.Setup(o => o.CurrentConverter).Returns(mockCurrentConverter.Object);

            mockSavegame = new Mock<IRequiredFile>();
            mockCurrentConverter.Setup(c => c.AbsoluteSourceSaveGame).Returns(mockSavegame.Object);

            testTarget = new ExtractSaveOperation(
                mockConverterOptions.Object,
                mockCompressedSaveChecker.Object,
                zipFileHelper.Object,
                mockEnvironment.Object);
        }

        [TestMethod]
        public void CanRun_ValidAbsoluteSavePathTest()
        {
            var testPath = "x:\\savegame.eu4";
            mockSavegame.Setup(s => s.SelectedValue).Returns(testPath);
            mockCompressedSaveChecker.Setup(sc => sc.IsCompressedSave(It.Is<string>(path => path.Equals(testPath))))
                .Returns(true);
            Assert.IsTrue(testTarget.CanRun());
        }

        [TestMethod]
        public void CanRun_InvalidAbsoluteSavePathTest()
        {
            Assert.IsFalse(testTarget.CanRun());
        }
    }
}