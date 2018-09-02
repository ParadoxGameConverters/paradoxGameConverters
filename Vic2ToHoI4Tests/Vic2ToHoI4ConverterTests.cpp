#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Vic2ToHoI4/Source/Configuration.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;


void setOutputName(const std::string& V2SaveFileName);



namespace ConverterMainTests
{

TEST_CLASS(setOutputNameTests)
{
	public:
		TEST_METHOD(filenameEmptyWhenGivenNoInput)
		{
			std::string inputName = "";
			theConfiguration.setOutputName("");
			setOutputName(inputName);
			Assert::AreEqual(std::string(""), theConfiguration.getOutputName());
		}
		TEST_METHOD(filenameCorrectWhenSimplyAName)
		{
			std::string inputName = "hoi4.v2";
			theConfiguration.setOutputName("");
			setOutputName(inputName);
			Assert::AreEqual(std::string("hoi4"), theConfiguration.getOutputName());
		}
		TEST_METHOD(filenameExtractedWithWindowsPath)
		{
			std::string inputName = "C:\\Users\\Cosmostarr\\Documents\\Paradox Interactive\\Victoria II\\save games\\hoi4.v2";
			theConfiguration.setOutputName("");
			setOutputName(inputName);
			Assert::AreEqual(std::string("hoi4"), theConfiguration.getOutputName());
		}
		TEST_METHOD(filenameExtractedWithLinuxPath)
		{
			std::string inputName = "/Users/Cosmostarr/Documents/Paradox Interactive/Victoria II/save games/hoi4.v2";
			theConfiguration.setOutputName("");
			setOutputName(inputName);
			Assert::AreEqual(std::string("hoi4"), theConfiguration.getOutputName());
		}
		TEST_METHOD(filenameExtractedWithMixedPathEndingLinuxStyle)
		{
			std::string inputName = "C:\\Users\\Cosmostarr\\Documents\\Paradox Interactive\\Victoria II\\save games/hoi4.v2";
			theConfiguration.setOutputName("");
			setOutputName(inputName);
			Assert::AreEqual(std::string("hoi4"), theConfiguration.getOutputName());
		}
		TEST_METHOD(filenameExtractedWithMixedPathEndingWindowsStyle)
		{
			std::string inputName = "/Users/Cosmostarr/Documents/Paradox Interactive/Victoria II/save games\\hoi4.v2";
			theConfiguration.setOutputName("");
			setOutputName(inputName);
			Assert::AreEqual(std::string("hoi4"), theConfiguration.getOutputName());
		}
		TEST_METHOD(filenameHasDashesReplaced)
		{
			std::string inputName = "hoi4-something.v2";
			theConfiguration.setOutputName("");
			setOutputName(inputName);
			Assert::AreEqual(std::string("hoi4_something"), theConfiguration.getOutputName());
		}
		TEST_METHOD(filenameHasSpacesReplaced)
		{
			std::string inputName = "hoi4 something.v2";
			theConfiguration.setOutputName("");
			setOutputName(inputName);
			Assert::AreEqual(std::string("hoi4_something"), theConfiguration.getOutputName());
		}
		TEST_METHOD(filenameHasNoExtension)
		{
			std::string inputName = "hoi4";
			auto lambda = [inputName](){ setOutputName(inputName); };

			Assert::ExpectException<std::exception>(lambda);
		}
		TEST_METHOD(filenameHasAnInvalidExtension)
		{
			std::string inputName = "hoi4.eu4";
			auto lambda = [inputName](){ setOutputName(inputName); };

			Assert::ExpectException<std::exception>(lambda);
		}
};

}