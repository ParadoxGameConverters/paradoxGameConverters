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
#include "../Vic2ToHoI4/Source/HOI4World/AllMilitaryMappings.h"



using namespace Microsoft::VisualStudio::CppUnitTestFramework;



namespace HoI4WorldTests
{

TEST_CLASS(allMilitaryMappingsTests)
{
	public:
		TEST_METHOD(getDefaultMappingsWithNoMods)
		{
			std::vector<std::string> mods;
			auto specificMappings = allTheMappings.getMilitaryMappings(mods);
			Assert::AreEqual(std::string("default"), specificMappings.getMappingsName());
		}
		TEST_METHOD(getDefaultMappingsWithInvalidMod)
		{
			std::vector<std::string> mods = { "NotAMod" };
			auto specificMappings = allTheMappings.getMilitaryMappings(mods);
			Assert::AreEqual(std::string("default"), specificMappings.getMappingsName());
		}
		TEST_METHOD(getPDMMappingsWithPDM)
		{
			std::vector<std::string> mods = { "PDM" };
			auto specificMappings = allTheMappings.getMilitaryMappings(mods);
			Assert::AreEqual(std::string("PDM"), specificMappings.getMappingsName());
		}

	private:
		HoI4::allMilitaryMappings allTheMappings;
};

}