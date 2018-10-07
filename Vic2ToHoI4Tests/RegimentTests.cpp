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
#include "../Vic2ToHoI4/Source/HoI4World/Regiment.h"



using namespace Microsoft::VisualStudio::CppUnitTestFramework;



namespace HoI4WorldTests
{

TEST_CLASS(RegimentTests)
{
	public:
		TEST_METHOD(RegimentTypeTypeDefaultsToBlank)
		{
			std::istringstream input("");
			HoI4::RegimentType regimentType("", input);
			Assert::AreEqual(std::string(""), regimentType.getType());
		}
		TEST_METHOD(RegimentTypeTypeCanBeImported)
		{
			std::istringstream input("= { x = 0 y = 0 }");
			HoI4::RegimentType regimentType("infantry", input);
			Assert::AreEqual(std::string("infantry"), regimentType.getType());
		}
		TEST_METHOD(RegimentTypeTypeCopiedByCopyConstructor)
		{
			std::istringstream input("= { x = 0 y = 0 }");
			HoI4::RegimentType regimentType("infantry", input);
			HoI4::RegimentType regimentType2(regimentType);
			Assert::AreEqual(regimentType.getType(), regimentType2.getType());
		}
		TEST_METHOD(BlankRegimentTypeOutputsProperly)
		{
			std::istringstream input("");
			HoI4::RegimentType regimentType("", input);
			std::ostringstream output;
			output << regimentType;
			Assert::AreEqual(std::string("\t\t = { x = 0 y = 0 }\n"), output.str());
		}
		TEST_METHOD(ImportedRegimentTypeOutputsProperly)
		{
			std::istringstream input("= { x = 1 y = 2 }");
			HoI4::RegimentType regimentType("infantry", input);
			std::ostringstream output;
			output << regimentType;
			Assert::AreEqual(std::string("\t\tinfantry = { x = 1 y = 2 }\n"), output.str());
		}
		TEST_METHOD(CopiedRegimentTypeOutputsProperly)
		{
			std::istringstream input("= { x = 1 y = 2 }");
			HoI4::RegimentType regimentType("infantry", input);
			std::ostringstream output;
			output << regimentType;
			HoI4::RegimentType regimentType2(regimentType);
			std::ostringstream output2;
			output2 << regimentType2;
			Assert::AreEqual(output.str(), output2.str());
		}
};

}