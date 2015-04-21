using Frontend.Core.Common.Proxies;
using Frontend.Core.Converting.Operations.ExtractSave;
using Frontend.Core.Helpers;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.Model.Paths.Interfaces;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Frontend.Core.Tests.Converter.Operations.ExtractSave
{
    [TestClass]
    public class ExtractSaveOperationTest : UnitTestBase
    {
        private ExtractSaveOperation testTarget;
        private Mock<IConverterOptions> mockConverterOptions;
        private Mock<ICompressedSaveChecker> mockCompressedSaveChecker;
        private Mock<IDirectoryHelper> mockDirectoryHelper;
        private Mock<IMessageBoxProxy> mockMessageBoxProxy;
        private Mock<IConverterSettings> mockCurrentConverter;
        private Mock<IRequiredFile> mockSavegame;
        private Mock<IFileProxy> fileProxy;
        private Mock<IZipFileHelper> zipFileHelper;
        private Mock<IEnvironmentProxy> mockEnvironment;

        protected override void OnCleanup()
        {
            this.mockSavegame = null;
            this.mockCurrentConverter = null;
            this.testTarget = null;
            this.mockMessageBoxProxy = null;
            this.mockDirectoryHelper = null;
            this.mockConverterOptions = null;
            this.mockCompressedSaveChecker = null;
            this.fileProxy = null;
            this.zipFileHelper = null;
            this.mockEnvironment = null;
            base.OnCleanup();
        }

        protected override void OnInitialize()
        {
            base.OnInitialize();
            this.mockCompressedSaveChecker = new Mock<ICompressedSaveChecker>();
            this.mockConverterOptions = new Mock<IConverterOptions>();
            this.mockDirectoryHelper = new Mock<IDirectoryHelper>();
            this.mockMessageBoxProxy = new Mock<IMessageBoxProxy>();
            this.fileProxy = new Mock<IFileProxy>();
            this.zipFileHelper = new Mock<IZipFileHelper>();
            this.mockEnvironment = new Mock<IEnvironmentProxy>();

            this.mockCurrentConverter = new Mock<IConverterSettings>();
            this.mockConverterOptions.Setup(o => o.CurrentConverter).Returns(mockCurrentConverter.Object);

            this.mockSavegame = new Mock<IRequiredFile>();
            this.mockCurrentConverter.Setup(c => c.AbsoluteSourceSaveGame).Returns(mockSavegame.Object);

            this.testTarget = new ExtractSaveOperation(
                this.mockConverterOptions.Object, 
                this.mockCompressedSaveChecker.Object, 
                this.zipFileHelper.Object,
                this.mockEnvironment.Object);
        }
        
        [TestMethod]
        public void CanRun_ValidAbsoluteSavePathTest()
        {
            string testPath = "x:\\savegame.eu4";
            this.mockSavegame.Setup(s => s.SelectedValue).Returns(testPath);
            this.mockCompressedSaveChecker.Setup(sc => sc.IsCompressedSave(It.Is<string>(path => path.Equals(testPath)))).Returns(true);
            Assert.IsTrue(this.testTarget.CanRun());
        }

        [TestMethod]
        public void CanRun_InvalidAbsoluteSavePathTest()
        {
            Assert.IsFalse(this.testTarget.CanRun());
        }
    }
}
