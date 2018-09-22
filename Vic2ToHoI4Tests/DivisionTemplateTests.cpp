/*Copyright (c) 2018 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Vic2ToHoI4/Source/HoI4World/DivisionTemplate.h"



using namespace Microsoft::VisualStudio::CppUnitTestFramework;



namespace HoI4WorldTests
{

TEST_CLASS(DivisionTemplateTests)
{
	public:
		TEST_METHOD(DivisionTempateNameDefaultsToBlank)
		{
			std::istringstream input("");
			HoI4::DivisionTemplateType divisionTemplate(input);
			Assert::AreEqual(std::string(""), divisionTemplate.getName());
		}
		TEST_METHOD(DivisionTempateNameCanBeImported)
		{
			std::istringstream input("= {\n"\
												"\t\t\tname = \"Light Infantry Brigade\"\n"\
												"\t\t}\n");
			HoI4::DivisionTemplateType divisionTemplate(input);
			Assert::AreEqual(std::string("Light Infantry Brigade"), divisionTemplate.getName());
		}
		TEST_METHOD(DivisionTempateNameCopiedWithCopyConstructor)
		{
			std::istringstream input("= {\n"\
												"\t\t\tname = \"Light Infantry Brigade\"\n"\
												"\t\t}\n");
			HoI4::DivisionTemplateType divisionTemplate(input);
			HoI4::DivisionTemplateType divisionTemplate2(divisionTemplate);
			Assert::AreEqual(divisionTemplate.getName(), divisionTemplate2.getName());
		}
		TEST_METHOD(DivisionTempateRegimentsDefaultToEmpty)
		{
			std::istringstream input("");
			HoI4::DivisionTemplateType divisionTemplate(input);
			Assert::AreEqual(size_t(0), divisionTemplate.getRegiments().size());
		}
		TEST_METHOD(DivisionTempateRegimentsCanBeImported)
		{
			std::istringstream input("= {\n"\
												"\t\t\tregiments = {\n"\
												"\t\t\t\tinfantry = { x = 0 y = 0 }\n"\
												"\t\t\t}\n"\
												"\t\t}\n");
			HoI4::DivisionTemplateType divisionTemplate(input);
			Assert::AreEqual(size_t(1), divisionTemplate.getRegiments().size());
		}
		TEST_METHOD(DivisionTempateRegimentsCopiedWithCopyConstructor)
		{
			std::istringstream input("= {\n"\
												"\t\t\tregiments = {\n"\
												"\t\t\t\tinfantry = { x = 0 y = 0 }\n"\
												"\t\t\t}\n"\
												"\t\t}\n");
			HoI4::DivisionTemplateType divisionTemplate(input);
			HoI4::DivisionTemplateType divisionTemplate2(divisionTemplate);
			Assert::AreEqual(divisionTemplate.getRegiments().size(), divisionTemplate2.getRegiments().size());
		}
		TEST_METHOD(DivisionTempateSupportRegimentsDefaultToEmpty)
		{
			std::istringstream input("");
			HoI4::DivisionTemplateType divisionTemplate(input);
			Assert::AreEqual(size_t(0), divisionTemplate.getSupportRegiments().size());
		}
		TEST_METHOD(DivisionTempateSupportRegimentsCanBeImported)
		{
			std::istringstream input("= {\n"\
												"\t\t\tsupport = {\n"\
												"\t\t\t\tinfantry = { x = 0 y = 0 }\n"\
												"\t\t\t}\n"\
												"\t\t}\n");
			HoI4::DivisionTemplateType divisionTemplate(input);
			Assert::AreEqual(size_t(1), divisionTemplate.getSupportRegiments().size());
		}
		TEST_METHOD(DivisionTempateSupportRegimentsCopiedWithCopyConstructor)
		{
			std::istringstream input("= {\n"\
												"\t\t\tsupport = {\n"\
												"\t\t\t\tinfantry = { x = 0 y = 0 }\n"\
												"\t\t\t}\n"\
												"\t\t}\n");
			HoI4::DivisionTemplateType divisionTemplate(input);
			HoI4::DivisionTemplateType divisionTemplate2(divisionTemplate);
			Assert::AreEqual(divisionTemplate.getSupportRegiments().size(), divisionTemplate2.getSupportRegiments().size());
		}
		TEST_METHOD(BlankDivisionTemplateOutputsProperly)
		{
			std::istringstream input("");
			HoI4::DivisionTemplateType divisionTemplate(input);
			std::ostringstream output;
			output << divisionTemplate;

			std::string outputString("division_template = {\n"\
												"\tname = \"\"\n"\
												"\n"\
												"\tregiments = {\n"\
												"\t}\n"\
												"\tsupport = {\n"\
												"\t}\n"\
												"}\n"
			);
			Assert::AreEqual(outputString, output.str());
		}
		TEST_METHOD(ImportedDivisionTemplateOutputsProperly)
		{
			std::istringstream input("= {\n"\
												"\t\t\tname = \"Light Infantry Brigade\"\n"\
												"\t\t\tregiments = {\n"\
												"\t\t\t\tinfantry = { x = 0 y = 0 }\n"\
												"\t\t\t}\n"\
												"\t\t\tsupport = {\n"\
												"\t\t\t\trecon = { x = 0 y = 0 }\n"\
												"\t\t\t}\n"\
												"\t\t}\n");
			HoI4::DivisionTemplateType divisionTemplate(input);
			std::ostringstream output;
			output << divisionTemplate;

			std::string outputString("division_template = {\n"\
												"\tname = \"Light Infantry Brigade\"\n"\
												"\n"\
												"\tregiments = {\n"\
												"\t\tinfantry = { x = 0 y = 0 }\n"\
												"\t}\n"\
												"\tsupport = {\n"\
												"\t\trecon = { x = 0 y = 0 }\n"\
												"\t}\n"\
												"}\n"
			);
			Assert::AreEqual(outputString, output.str());
		}
		TEST_METHOD(CopiedDivisionTemplateOutputsProperly)
		{
			std::istringstream input("");
			HoI4::DivisionTemplateType divisionTemplate(input);
			std::ostringstream output;
			output << divisionTemplate;

			std::string outputString("division_template = {\n"\
												"\tname = \"\"\n"\
												"\n"\
												"\tregiments = {\n"\
												"\t}\n"\
												"\tsupport = {\n"\
												"\t}\n"\
												"}\n"
			);
			Assert::AreEqual(outputString, output.str());
		}
		TEST_METHOD(DivisionTempatesWithDifferentNamesNotEqual)
		{
			std::istringstream input("= {\n"\
												"\t\t\tname = \"Light Infantry Brigade\"\n"\
												"\t\t}\n");
			HoI4::DivisionTemplateType divisionTemplate(input);
			Assert::IsFalse(divisionTemplate == std::string("Light Infantry Brigade2"));
		}
		TEST_METHOD(DivisionTempatesWithSameNamesEqual)
		{
			std::istringstream input("= {\n"\
												"\t\t\tname = \"Light Infantry Brigade\"\n"\
												"\t\t}\n");
			HoI4::DivisionTemplateType divisionTemplate(input);
			Assert::IsTrue(divisionTemplate == std::string("Light Infantry Brigade"));
		}
};

}