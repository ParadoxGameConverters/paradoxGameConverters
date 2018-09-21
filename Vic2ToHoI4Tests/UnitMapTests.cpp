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
#include "../Vic2ToHoI4/Source/HOI4World/UnitMap.h"



using namespace Microsoft::VisualStudio::CppUnitTestFramework;



namespace HoI4WorldTests
{

TEST_CLASS(UnitMapTests)
{
	public:
		TEST_METHOD(defaultUnitMapCategoryIsBlank)
		{
			HoI4::UnitMap theMap;
			Assert::AreEqual(std::string(""), theMap.getCategory());
		}
		TEST_METHOD(defaultUnitMapTypeIsBlank)
		{
			HoI4::UnitMap theMap;
			Assert::AreEqual(std::string(""), theMap.getType());
		}
		TEST_METHOD(defaultUnitMapEquipmentIsBlank)
		{
			HoI4::UnitMap theMap;
			Assert::AreEqual(std::string(""), theMap.getEquipment());
		}
		TEST_METHOD(defaultUnitMapSizeIsZero)
		{
			HoI4::UnitMap theMap;
			Assert::AreEqual(0, theMap.getSize());
		}
		TEST_METHOD(blankUnitMapCategoryRemainsBlank)
		{
			std::stringstream input(
				"= {\n"\
				"\t\t\t}"
			);
			HoI4::UnitMap theMap(input);
			Assert::AreEqual(std::string(""), theMap.getCategory());
		}
		TEST_METHOD(blankUnitMapTypeRemainsBlank)
		{
			std::stringstream input(
				"= {\n"\
				"\t\t\t}"
			);
			HoI4::UnitMap theMap(input);
			Assert::AreEqual(std::string(""), theMap.getType());
		}
		TEST_METHOD(blankUnitMapEquipmentRemainsBlank)
		{
			std::stringstream input(
				"= {\n"\
				"\t\t\t}"
			);
			HoI4::UnitMap theMap(input);
			Assert::AreEqual(std::string(""), theMap.getEquipment());
		}
		TEST_METHOD(blankUnitMapSizeRemainsZero)
		{
			std::stringstream input(
				"= {\n"\
				"\t\t\t}"
			);
			HoI4::UnitMap theMap(input);
			Assert::AreEqual(0, theMap.getSize());
		}
		TEST_METHOD(UnitMapCategoryCanBeSet)
		{
			std::stringstream input(
				"= {\n"\
				"\t\t\t\tcategory = land\n"\
				"\t\t\t}"
			);
			HoI4::UnitMap theMap(input);
			Assert::AreEqual(std::string("land"), theMap.getCategory());
		}
		TEST_METHOD(UnitMapTypeCanBeSet)
		{
			std::stringstream input(
				"= {\n"\
				"\t\t\t\ttype = infantry\n"\
				"\t\t\t}"
			);
			HoI4::UnitMap theMap(input);
			Assert::AreEqual(std::string("infantry"), theMap.getType());
		}
		TEST_METHOD(UnitMapEquipmentCanBeSet)
		{
			std::stringstream input(
				"= {\n"\
				"\t\t\t\tequipment = infantry_equipment_0\n"\
				"\t\t\t}"
			);
			HoI4::UnitMap theMap(input);
			Assert::AreEqual(std::string("infantry_equipment_0"), theMap.getEquipment());
		}
		TEST_METHOD(UnitMapSizeCanBeSet)
		{
			std::stringstream input(
				"= {\n"\
				"\t\t\t\tsize = 3\n"\
				"\t\t\t}"
			);
			HoI4::UnitMap theMap(input);
			Assert::AreEqual(3, theMap.getSize());
		}
};

}