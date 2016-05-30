using System.Collections.Generic;
using System.IO;
using Caliburn.Micro;
using Frontend.Core.Common.Proxies;
using Frontend.Core.Converting.Operations.CopyMod;
using Frontend.Core.Helpers;
using Frontend.Core.Model.Interfaces;
using Frontend.Core.Model.Paths.Interfaces;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;

namespace Frontend.Core.Tests.Converting
{
    [TestClass]
    public class ModCopierTest : UnitTestBase
    {
        private readonly string dotModPostfix = ".mod";
        private readonly string modFolderSourcePathMinusDesiredFileName = "x:\\converter\\output\\";
        private readonly string targetGameModPathTagName = "targetGameModPath";
        private readonly string vic2ModPath = "x:\\vic2\\mod\\";
        private readonly string workingDirectory = "x:\\converter\\";
        private Mock<IConverterOptions> mockConverterOptions;
        private Mock<IDirectoryCopyHelper> mockDirectoryCopyHelper;
        private Mock<IDirectoryHelper> mockDirectoryHelper;
        private Mock<IEventAggregator> mockEventAggregator;
        private Mock<IFileProxy> mockFileProxy;
        private Mock<IFolderProxy> mockFolderProxy;
        private Mock<IMessageBoxProxy> mockMessageBoxProxy;
        private Mock<ISaveGameNameTranslator> mockSaveGameTranslator;
        private ModCopier modCopier;
        //string modFileSourcePath = "modFileSourcePath";

        protected override void OnInitialize()
        {
            base.OnInitialize();
            mockEventAggregator = new Mock<IEventAggregator>();
            mockConverterOptions = new Mock<IConverterOptions>();
            mockFileProxy = new Mock<IFileProxy>();
            mockFolderProxy = new Mock<IFolderProxy>();
            mockMessageBoxProxy = new Mock<IMessageBoxProxy>();
            mockDirectoryCopyHelper = new Mock<IDirectoryCopyHelper>();
            mockDirectoryHelper = new Mock<IDirectoryHelper>();
            mockSaveGameTranslator = new Mock<ISaveGameNameTranslator>();

            var mockConverterSettings = new Mock<IConverterSettings>();
            var requiredItems = new List<IRequiredItemBase>();

            mockConverterOptions.Setup(o => o.CurrentConverter).Returns(mockConverterSettings.Object);
            mockConverterSettings.Setup(s => s.RequiredItems).Returns(requiredItems);
            var mockTargetGameModPathItem = new Mock<IRequiredItemBase>();
            mockTargetGameModPathItem.Setup(i => i.InternalTagName).Returns(targetGameModPathTagName);
            mockTargetGameModPathItem.Setup(i => i.SelectedValue).Returns(vic2ModPath);
            requiredItems.Add(mockTargetGameModPathItem.Object);
            var mockSaveGame = new Mock<IRequiredFile>();
            mockConverterSettings.Setup(s => s.AbsoluteSourceSaveGame).Returns(mockSaveGame.Object);

            var mockTargetGame = new Mock<IGameConfiguration>();
            mockConverterSettings.Setup(s => s.TargetGame).Returns(mockTargetGame.Object);

            mockDirectoryHelper.Setup(d => d.GetConverterWorkingDirectory(It.IsAny<IConverterSettings>()))
                .Returns(workingDirectory);

            mockFileProxy.Setup(p => p.Exists(It.IsAny<string>())).Returns(false);
            mockFolderProxy.Setup(p => p.Exists(It.IsAny<string>())).Returns(false);

            modCopier = new ModCopier(
                mockConverterOptions.Object,
                mockFileProxy.Object,
                mockFolderProxy.Object,
                mockDirectoryHelper.Object,
                mockMessageBoxProxy.Object,
                mockDirectoryCopyHelper.Object,
                mockSaveGameTranslator.Object);
        }

        protected override void OnCleanup()
        {
            base.OnCleanup();
            modCopier = null;
            mockEventAggregator = null;
            mockConverterOptions = null;
            mockFileProxy = null;
            mockFolderProxy = null;
            mockMessageBoxProxy = null;
            mockDirectoryCopyHelper = null;
            mockDirectoryHelper = null;
        }

        [TestMethod]
        public void Test_Moving_File_And_Folder_With_Spaces_And_Dashes_In_The_Name()
        {
            // Arrange

            var saveNameWithSpaces = "Version_1_9_end - Copy";
            var translatedSaveGameName = "Version_1_9_end___Copy";
            var modFileName = string.Concat(saveNameWithSpaces + dotModPostfix);
            var translatedModFileName = string.Concat(translatedSaveGameName + dotModPostfix);
            var targetFilePath = string.Concat(vic2ModPath, translatedSaveGameName, dotModPostfix);
            var targetFolderPath = string.Concat(vic2ModPath, translatedSaveGameName);

            mockFolderProxy.Setup(p => p.GetFileNameWithoutExtension(It.IsAny<string>())).Returns(saveNameWithSpaces);

            mockSaveGameTranslator.Setup(t => t.TranslateName(It.Is<string>(s => s.Equals(saveNameWithSpaces))))
                .Returns(translatedSaveGameName);

            /* Match
             * var outputModFolderSourcePath = this.folderProxy.Combine(converterWorkingDirectory, "output", desiredFileName);
             */
            mockFolderProxy.Setup(p => p.Combine(
                It.Is<string>(s => s.Equals(workingDirectory)),
                It.Is<string>(s => s.Equals("output")),
                It.Is<string>(s => s.Equals(translatedSaveGameName))))
                .Returns(Path.Combine(modFolderSourcePathMinusDesiredFileName, translatedSaveGameName));
            /* Match
             * var outputModFileSourcePath = this.folderProxy.Combine(converterWorkingDirectory, "output", (desiredFileName + ".mod"));
             */
            mockFolderProxy.Setup(p => p.Combine(
                It.Is<string>(s => s.Equals(workingDirectory)),
                It.Is<string>(s => s.Equals("output")),
                It.Is<string>(s => s.Equals(translatedModFileName))))
                .Returns(Path.Combine(workingDirectory, "output", translatedModFileName));

            /* Match
             * var expectedAbsoluteOutputModFolderTargetPath = this.folderProxy.Combine(targetGameModPathItem.SelectedValue, desiredFileName);
             */
            mockFolderProxy.Setup(p => p.Combine(
                It.Is<string>(s => s.Equals(vic2ModPath)),
                It.Is<string>(s => s.Equals(translatedSaveGameName))))
                .Returns(Path.Combine(vic2ModPath, translatedSaveGameName));

            // If the name isn't translated.
            mockFileProxy.Setup(p => p.Copy(
                It.Is<string>(s => s.Equals(Path.Combine(workingDirectory, "output", translatedModFileName))),
                It.Is<string>(s => s.Equals(targetFilePath)),
                It.Is<bool>(b => b))).Throws(new FileNotFoundException());

            // If the name is translated
            mockFileProxy.Setup(p => p.Copy(
                It.Is<string>(s => s.Equals(Path.Combine(workingDirectory, "output", translatedModFileName))),
                It.Is<string>(s => s.Equals(targetFilePath)),
                It.Is<bool>(b => b)));

            mockDirectoryCopyHelper.Setup(h => h.DirectoryCopy(
                It.Is<string>(
                    s => s.Equals(Path.Combine(modFolderSourcePathMinusDesiredFileName, translatedSaveGameName))),
                It.Is<string>(s => s.Equals(targetFolderPath)),
                It.Is<bool>(b => b),
                It.Is<bool>(b => b)));

            // Act
            modCopier.MoveModFileAndFolder();

            // Assert
            mockFolderProxy.VerifyAll();
            mockFileProxy.VerifyAll();
            mockDirectoryHelper.VerifyAll();
            mockDirectoryCopyHelper.VerifyAll();
            mockEventAggregator.VerifyAll();
        }

        [TestMethod]
        public void Test_Moving_File_And_Folder()
        {
            // Arrange
            var normalSaveName = "Version_1_9_end";
            var modFileName = string.Concat(normalSaveName + dotModPostfix);
            var targetFilePath = string.Concat(vic2ModPath, normalSaveName, dotModPostfix);
            var targetFolderPath = string.Concat(vic2ModPath, normalSaveName);

            mockFolderProxy.Setup(p => p.GetFileNameWithoutExtension(It.IsAny<string>())).Returns(normalSaveName);

            /* Match
             * var outputModFolderSourcePath = this.folderProxy.Combine(converterWorkingDirectory, "output", desiredFileName);
             */
            mockFolderProxy.Setup(p => p.Combine(
                It.Is<string>(s => s.Equals(workingDirectory)),
                It.Is<string>(s => s.Equals("output")),
                It.Is<string>(s => s.Equals(normalSaveName))))
                .Returns(Path.Combine(modFolderSourcePathMinusDesiredFileName, normalSaveName));
            /* Match
             * var outputModFileSourcePath = this.folderProxy.Combine(converterWorkingDirectory, "output", (desiredFileName + ".mod"));
             */
            mockFolderProxy.Setup(p => p.Combine(
                It.Is<string>(s => s.Equals(workingDirectory)),
                It.Is<string>(s => s.Equals("output")),
                It.Is<string>(s => s.Equals(modFileName))))
                .Returns(Path.Combine(workingDirectory, "output", modFileName));

            /* Match
             * var expectedAbsoluteOutputModFolderTargetPath = this.folderProxy.Combine(targetGameModPathItem.SelectedValue, desiredFileName);
             */
            mockFolderProxy.Setup(p => p.Combine(
                It.Is<string>(s => s.Equals(vic2ModPath)),
                It.Is<string>(s => s.Equals(normalSaveName))))
                .Returns(Path.Combine(vic2ModPath, normalSaveName));

            mockSaveGameTranslator.Setup(t => t.TranslateName(It.Is<string>(s => s.Equals(normalSaveName))))
                .Returns(normalSaveName);

            mockFileProxy.Setup(p => p.Copy(
                It.Is<string>(s => s.Equals(Path.Combine(workingDirectory, "output", modFileName))),
                It.Is<string>(s => s.Equals(targetFilePath)),
                It.Is<bool>(b => b)));

            mockDirectoryCopyHelper.Setup(h => h.DirectoryCopy(
                It.Is<string>(s => s.Equals(Path.Combine(modFolderSourcePathMinusDesiredFileName, normalSaveName))),
                It.Is<string>(s => s.Equals(targetFolderPath)),
                It.Is<bool>(b => b),
                It.Is<bool>(b => b)));

            // Act
            modCopier.MoveModFileAndFolder();

            // Assert
            mockFolderProxy.VerifyAll();
            mockFileProxy.VerifyAll();
            mockDirectoryHelper.VerifyAll();
            mockDirectoryCopyHelper.VerifyAll();
            mockEventAggregator.VerifyAll();
        }
    }
}