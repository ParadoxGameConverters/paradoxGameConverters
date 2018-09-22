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
#include "../Vic2ToHoI4/Source/HoI4World/Division.h"



using namespace Microsoft::VisualStudio::CppUnitTestFramework;



namespace HoI4WorldTests
{

TEST_CLASS(DivisionTests)
{
	public:
		TEST_METHOD(BlankDivisionTypeOutputsProperly)
		{
			HoI4::DivisionType division("", "", 0);
			std::ostringstream output;
			output << division;

			std::ostringstream expectedOutput;
			expectedOutput << "\tdivision = {\n";
			expectedOutput << "\t\tname = \"\"\n";
			expectedOutput << "\t\tlocation = 0\n";
			expectedOutput << "\t\tdivision_template = \"\"\n";
			expectedOutput << "\t\tstart_experience_factor = 0.3\n";
			expectedOutput << "\t\tstart_equipment_factor = 0.7\n";
			expectedOutput << "\t}\n";
			Assert::AreEqual(expectedOutput.str(), output.str());
		}
		TEST_METHOD(ConfiguredDivisionTypeOutputsProperly)
		{
			HoI4::DivisionType division("Test Division", "Light Infantry Brigade", 42);
			std::ostringstream output;
			output << division;

			std::ostringstream expectedOutput;
			expectedOutput << "\tdivision = {\n";
			expectedOutput << "\t\tname = \"Test Division\"\n";
			expectedOutput << "\t\tlocation = 42\n";
			expectedOutput << "\t\tdivision_template = \"Light Infantry Brigade\"\n";
			expectedOutput << "\t\tstart_experience_factor = 0.3\n";
			expectedOutput << "\t\tstart_equipment_factor = 0.7\n";
			expectedOutput << "\t}\n";
			Assert::AreEqual(expectedOutput.str(), output.str());
		}
		TEST_METHOD(CopiedDivisionTypeOutputsProperly)
		{
			HoI4::DivisionType division("Test Division", "Light Infantry Brigade", 42);
			HoI4::DivisionType division2 = division;
			std::ostringstream output;
			output << division2;

			std::ostringstream expectedOutput;
			expectedOutput << "\tdivision = {\n";
			expectedOutput << "\t\tname = \"Test Division\"\n";
			expectedOutput << "\t\tlocation = 42\n";
			expectedOutput << "\t\tdivision_template = \"Light Infantry Brigade\"\n";
			expectedOutput << "\t\tstart_experience_factor = 0.3\n";
			expectedOutput << "\t\tstart_equipment_factor = 0.7\n";
			expectedOutput << "\t}\n";
			Assert::AreEqual(expectedOutput.str(), output.str());
		}
};

}