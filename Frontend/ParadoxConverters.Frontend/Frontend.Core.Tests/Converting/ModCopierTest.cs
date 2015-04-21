using Caliburn.Micro;
using Frontend.Core.Common.Proxies;
using Frontend.Core.Converting.Operations.CopyMod;
using Frontend.Core.Helpers;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.Model.Paths.Interfaces;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using System.Collections.Generic;
using System.IO;

namespace Frontend.Core.Tests.Converting
{
    [TestClass]
    public class ModCopierTest : UnitTestBase
    {
        private ModCopier modCopier;
        private Mock<IEventAggregator> mockEventAggregator;
        private Mock<IConverterOptions> mockConverterOptions;
        private Mock<IFileProxy> mockFileProxy;
        private Mock<IFolderProxy> mockFolderProxy;
        private Mock<IDirectoryHelper> mockDirectoryHelper;
        private Mock<IMessageBoxProxy> mockMessageBoxProxy;
        private Mock<IDirectoryCopyHelper> mockDirectoryCopyHelper;
        private Mock<ISaveGameNameTranslator> mockSaveGameTranslator;

        string targetGameModPathTagName = "targetGameModPath";
        string vic2ModPath = "x:\\vic2\\mod\\";
        string workingDirectory = "x:\\converter\\";
        string dotModPostfix = ".mod";
        string modFolderSourcePathMinusDesiredFileName = "x:\\converter\\output\\";
        //string modFileSourcePath = "modFileSourcePath";

        protected override void OnInitialize()
        {
            base.OnInitialize();
            this.mockEventAggregator = new Mock<IEventAggregator>();
            this.mockConverterOptions = new Mock<IConverterOptions>();
            this.mockFileProxy = new Mock<IFileProxy>();
            this.mockFolderProxy = new Mock<IFolderProxy>();
            this.mockMessageBoxProxy = new Mock<IMessageBoxProxy>();
            this.mockDirectoryCopyHelper = new Mock<IDirectoryCopyHelper>();
            this.mockDirectoryHelper = new Mock<IDirectoryHelper>();
            this.mockSaveGameTranslator = new Mock<ISaveGameNameTranslator>();

            var mockConverterSettings = new Mock<IConverterSettings>();
            var requiredItems = new List<IRequiredItemBase>();

            this.mockConverterOptions.Setup(o => o.CurrentConverter).Returns(mockConverterSettings.Object);
            mockConverterSettings.Setup(s => s.RequiredItems).Returns(requiredItems);
            var mockTargetGameModPathItem = new Mock<IRequiredItemBase>();
            mockTargetGameModPathItem.Setup(i => i.InternalTagName).Returns(this.targetGameModPathTagName);
            mockTargetGameModPathItem.Setup(i => i.SelectedValue).Returns(this.vic2ModPath);
            requiredItems.Add(mockTargetGameModPathItem.Object);
            var mockSaveGame = new Mock<IRequiredFile>();
            mockConverterSettings.Setup(s => s.AbsoluteSourceSaveGame).Returns(mockSaveGame.Object);

            var mockTargetGame = new Mock<IGameConfiguration>();
            mockConverterSettings.Setup(s => s.TargetGame).Returns(mockTargetGame.Object);

            this.mockDirectoryHelper.Setup(d => d.GetConverterWorkingDirectory(It.IsAny<IConverterSettings>())).Returns(this.workingDirectory);
            
            this.mockFileProxy.Setup(p => p.Exists(It.IsAny<string>())).Returns(false);
            this.mockFolderProxy.Setup(p => p.Exists(It.IsAny<string>())).Returns(false);

            this.modCopier = new ModCopier(
                this.mockConverterOptions.Object,
                this.mockFileProxy.Object,
                this.mockFolderProxy.Object, 
                this.mockDirectoryHelper.Object, 
                this.mockMessageBoxProxy.Object, 
                this.mockDirectoryCopyHelper.Object,
                this.mockSaveGameTranslator.Object);
        }

        protected override void OnCleanup()
        {
            base.OnCleanup();
            this.modCopier = null;
            this.mockEventAggregator = null;
            this.mockConverterOptions = null;
            this.mockFileProxy = null;
            this.mockFolderProxy = null;
            this.mockMessageBoxProxy = null;
            this.mockDirectoryCopyHelper = null;
            this.mockDirectoryHelper = null;
        }

        [TestMethod]
        public void Test_Moving_File_And_Folder_With_Spaces_And_Dashes_In_The_Name()
        {
            // Arrange

            string saveNameWithSpaces = "Version_1_9_end - Copy";
            string translatedSaveGameName = "Version_1_9_end___Copy";
            string modFileName = string.Concat(saveNameWithSpaces + this.dotModPostfix);
            string translatedModFileName = string.Concat(translatedSaveGameName + this.dotModPostfix);
            string targetFilePath = string.Concat(this.vic2ModPath, translatedSaveGameName, this.dotModPostfix);
            string targetFolderPath = string.Concat(this.vic2ModPath, translatedSaveGameName);

            this.mockFolderProxy.Setup(p => p.GetFileNameWithoutExtension(It.IsAny<string>())).Returns(saveNameWithSpaces);

            this.mockSaveGameTranslator.Setup(t => t.TranslateName(It.Is<string>(s => s.Equals(saveNameWithSpaces)))).Returns(translatedSaveGameName);

            /* Match
             * var outputModFolderSourcePath = this.folderProxy.Combine(converterWorkingDirectory, "output", desiredFileName);
             */
            this.mockFolderProxy.Setup(p => p.Combine(
                It.Is<string>(s => s.Equals(this.workingDirectory)),
                It.Is<string>(s => s.Equals("output")),
                It.Is<string>(s => s.Equals(translatedSaveGameName))))
                .Returns(Path.Combine(this.modFolderSourcePathMinusDesiredFileName, translatedSaveGameName));
            /* Match
             * var outputModFileSourcePath = this.folderProxy.Combine(converterWorkingDirectory, "output", (desiredFileName + ".mod"));
             */
            this.mockFolderProxy.Setup(p => p.Combine(
                It.Is<string>(s => s.Equals(this.workingDirectory)),
                It.Is<string>(s => s.Equals("output")),
                It.Is<string>(s => s.Equals(translatedModFileName))))
                .Returns(Path.Combine(this.workingDirectory, "output", translatedModFileName));

            /* Match
             * var expectedAbsoluteOutputModFolderTargetPath = this.folderProxy.Combine(targetGameModPathItem.SelectedValue, desiredFileName);
             */
            this.mockFolderProxy.Setup(p => p.Combine(
                It.Is<string>(s => s.Equals(this.vic2ModPath)),
                It.Is<string>(s => s.Equals(translatedSaveGameName))))
                .Returns(Path.Combine(this.vic2ModPath, translatedSaveGameName));

            // If the name isn't translated.
            this.mockFileProxy.Setup(p => p.Copy(
                It.Is<string>(s => s.Equals(Path.Combine(this.workingDirectory, "output", translatedModFileName))),
                It.Is<string>(s => s.Equals(targetFilePath)),
                It.Is<bool>(b => b))).Throws(new FileNotFoundException());

            // If the name is translated
            this.mockFileProxy.Setup(p => p.Copy(
                It.Is<string>(s => s.Equals(Path.Combine(this.workingDirectory, "output", translatedModFileName))),
                It.Is<string>(s => s.Equals(targetFilePath)),
                It.Is<bool>(b => b)));

            this.mockDirectoryCopyHelper.Setup(h => h.DirectoryCopy(
                It.Is<string>(s => s.Equals(Path.Combine(this.modFolderSourcePathMinusDesiredFileName, translatedSaveGameName))),
                It.Is<string>(s => s.Equals(targetFolderPath)),
                It.Is<bool>(b => b),
                It.Is<bool>(b => b)));

            // Act
            this.modCopier.MoveModFileAndFolder();

            // Assert
            this.mockFolderProxy.VerifyAll();
            this.mockFileProxy.VerifyAll();
            this.mockDirectoryHelper.VerifyAll();
            this.mockDirectoryCopyHelper.VerifyAll();
            this.mockEventAggregator.VerifyAll();
        }

        [TestMethod]
        public void Test_Moving_File_And_Folder()
        {
            // Arrange
            string normalSaveName = "Version_1_9_end";
            string modFileName = string.Concat(normalSaveName + this.dotModPostfix);
            string targetFilePath = string.Concat(this.vic2ModPath, normalSaveName, this.dotModPostfix);
            string targetFolderPath = string.Concat(this.vic2ModPath, normalSaveName);
            
            this.mockFolderProxy.Setup(p => p.GetFileNameWithoutExtension(It.IsAny<string>())).Returns(normalSaveName);

            /* Match
             * var outputModFolderSourcePath = this.folderProxy.Combine(converterWorkingDirectory, "output", desiredFileName);
             */
            this.mockFolderProxy.Setup(p => p.Combine(
                It.Is<string>(s => s.Equals(this.workingDirectory)), 
                It.Is<string>(s => s.Equals("output")), 
                It.Is<string>(s => s.Equals(normalSaveName))))
                .Returns(Path.Combine(this.modFolderSourcePathMinusDesiredFileName, normalSaveName));
            /* Match
             * var outputModFileSourcePath = this.folderProxy.Combine(converterWorkingDirectory, "output", (desiredFileName + ".mod"));
             */
            this.mockFolderProxy.Setup(p => p.Combine(
                It.Is<string>(s => s.Equals(this.workingDirectory)), 
                It.Is<string>(s=> s.Equals("output")),
                It.Is<string>(s => s.Equals(modFileName))))
                .Returns(Path.Combine(this.workingDirectory, "output", modFileName));

            /* Match
             * var expectedAbsoluteOutputModFolderTargetPath = this.folderProxy.Combine(targetGameModPathItem.SelectedValue, desiredFileName);
             */
            this.mockFolderProxy.Setup(p => p.Combine(
                It.Is<string>(s => s.Equals(this.vic2ModPath)),
                It.Is<string>(s => s.Equals(normalSaveName))))
                .Returns(Path.Combine(this.vic2ModPath, normalSaveName));

            this.mockSaveGameTranslator.Setup(t => t.TranslateName(It.Is<string>(s => s.Equals(normalSaveName)))).Returns(normalSaveName);

            this.mockFileProxy.Setup(p => p.Copy(
                It.Is<string>(s => s.Equals(Path.Combine(this.workingDirectory, "output", modFileName))),
                It.Is<string>(s => s.Equals(targetFilePath)), 
                It.Is<bool>(b => b)));

            this.mockDirectoryCopyHelper.Setup(h => h.DirectoryCopy(
                It.Is<string>(s => s.Equals(Path.Combine(this.modFolderSourcePathMinusDesiredFileName, normalSaveName))),
                It.Is<string>(s => s.Equals(targetFolderPath)),
                It.Is<bool>(b => b),
                It.Is<bool>(b => b)));
            
            // Act
            this.modCopier.MoveModFileAndFolder();

            // Assert
            this.mockFolderProxy.VerifyAll();
            this.mockFileProxy.VerifyAll();
            this.mockDirectoryHelper.VerifyAll();
            this.mockDirectoryCopyHelper.VerifyAll();
            this.mockEventAggregator.VerifyAll();
        }
    }
}
