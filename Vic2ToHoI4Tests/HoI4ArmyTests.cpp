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
#include "../Vic2ToHoI4/Source/Configuration.h"
#include "../Vic2ToHoI4/Source/Mappers/ProvinceMapper.h"
#include "../Vic2ToHoI4/Source/HoI4World/HoI4Army.h"
#include "../Vic2ToHoI4/Source/V2World/Army.h"



using namespace Microsoft::VisualStudio::CppUnitTestFramework;



namespace HoI4WorldTests
{

TEST_CLASS(HoI4ArmyTests)
{
	public:
		TEST_CLASS_INITIALIZE(HoI4ArmyTestsInitialization)
		{
			ConfigurationFile("configuration.txt");
			theProvinceMapper.initialize();
		}

		TEST_METHOD(ArmyStartsEmpty)
		{
			HoI4::Army theArmy;

			std::ostringstream output;
			output << theArmy;
			Assert::AreEqual(std::string(""), output.str());
		}
		TEST_METHOD(EmptyArmyStaysEmpty)
		{
			HoI4::Army theArmy;

			std::stringstream mappingsInput;
			HoI4::militaryMappings theMilitaryMappings(std::string("default"), mappingsInput);
			theArmy.convertArmies(theMilitaryMappings, 0);

			std::ostringstream output;
			output << theArmy;
			Assert::AreEqual(std::string(""), output.str());
		}
		TEST_METHOD(InsufficientDivisionsBecomeNothing)
		{
			HoI4::Army theArmy;

			std::stringstream mappingsInput("= {\n"\
												 "\tmap = {\n"\
												 "\t\tlink = {\n"\
												 "\t\t\tvic = infantry\n"\
												 "\t\t\thoi = {\n"\
												 "\t\t\t\tcategory = land\n"\
												 "\t\t\t\ttype = infantry\n"\
												 "\t\t\t\tequipment = infantry_equipment_0\n"\
												 "\t\t\t\tsize = 3\n"\
												 "\t\t\t}\n"\
												 "\t\t}\n"\
												 "\t}\n"\
												 "\tdivision_templates = {\n"\
												 "\t\tdivision_template= {\n"\
												 "\t\t\tname = \"Light Infantry Brigade\"\n"\
												 "\t\t\tregiments = {\n"\
												 "\t\t\t\tinfantry = { x = 0 y = 0 }\n"\
												 "\t\t\t}\n"\
												 "\t\t}\n"\
												 "\t}\n"\
												 "}"
			);
			HoI4::militaryMappings theMilitaryMappings(std::string("default"), mappingsInput);
			theArmy.convertArmies(theMilitaryMappings, 0);

			std::ostringstream output;
			output << theArmy;
			Assert::AreEqual(std::string(""), output.str());
		}
		TEST_METHOD(SufficientDivisionsConvert)
		{
			HoI4::Army theArmy;
			std::vector<const Vic2::Army*> Vic2Armies;
			std::istringstream armyInput(	"=\n"\
													"\t{\n"\
													"\t\tname=\"I. Legio\"\n"\
													"\t\tlocation=496\n"\
													"\t\tregiment=\n"\
													"\t\t{\n"\
													"\t\t\texperience=30.000\n"\
													"\t\t\tcount=1\n"\
													"\t\t\ttype=infantry\n"\
													"\t\t}\n"\
													"\t}");
			Vic2::Army* Vic2Army = new Vic2::Army("army", armyInput);
			Vic2Armies.push_back(Vic2Army);
			theArmy.addSourceArmies(Vic2Armies);

			std::stringstream mappingsInput("= {\n"\
													  "\tmap = {\n"\
													  "\t\tlink = {\n"\
													  "\t\t\tvic = infantry\n"\
													  "\t\t\thoi = {\n"\
													  "\t\t\t\tcategory = land\n"\
													  "\t\t\t\ttype = infantry\n"\
													  "\t\t\t\tequipment = infantry_equipment_0\n"\
													  "\t\t\t\tsize = 3\n"\
													  "\t\t\t}\n"\
													  "\t\t}\n"\
													  "\t}\n"\
													  "\tdivision_templates = {\n"\
													  "\t\tdivision_template= {\n"\
													  "\t\t\tname = \"Light Infantry Brigade\"\n"\
													  "\t\t\tregiments = {\n"\
													  "\t\t\t\tinfantry = { x = 0 y = 0 }\n"\
													  "\t\t\t}\n"\
													  "\t\t}\n"\
													  "\t}\n"\
													  "}"
			);
			HoI4::militaryMappings theMilitaryMappings(std::string("default"), mappingsInput);
			theArmy.convertArmies(theMilitaryMappings, 0);

			std::ostringstream output;
			output << theArmy;
			Assert::AreEqual(std::string("\tdivision = {\n"\
													"\t\tname = \"1. Light Infantry Brigade\"\n"\
													"\t\tlocation = 11821\n"\
													"\t\tdivision_template = \"Light Infantry Brigade\"\n"\
													"\t\tstart_experience_factor = 0.3\n"\
													"\t\tstart_equipment_factor = 0.7\n"\
													"\t}\n"\
													"\tdivision = {\n"\
													"\t\tname = \"2. Light Infantry Brigade\"\n"\
													"\t\tlocation = 11821\n"\
													"\t\tdivision_template = \"Light Infantry Brigade\"\n"\
													"\t\tstart_experience_factor = 0.3\n"\
													"\t\tstart_equipment_factor = 0.7\n"\
													"\t}\n"\
													"\tdivision = {\n"\
													"\t\tname = \"3. Light Infantry Brigade\"\n"\
													"\t\tlocation = 11821\n"\
													"\t\tdivision_template = \"Light Infantry Brigade\"\n"\
													"\t\tstart_experience_factor = 0.3\n"\
													"\t\tstart_equipment_factor = 0.7\n"\
													"\t}\n"), output.str());

			delete Vic2Army;
		}
		TEST_METHOD(DivisionsCanMapToLaterTemplate)
		{
			HoI4::Army theArmy;
			std::vector<const Vic2::Army*> Vic2Armies;
			std::istringstream armyInput(	"=\n"\
												  "\t{\n"\
												  "\t\tname=\"I. Legio\"\n"\
												  "\t\tlocation=496\n"\
												  "\t\tregiment=\n"\
												  "\t\t{\n"\
												  "\t\t\texperience=30.000\n"\
												  "\t\t\tcount=1\n"\
												  "\t\t\ttype=infantry\n"\
												  "\t\t}\n"\
												  "\t}");
			Vic2::Army* Vic2Army = new Vic2::Army("army", armyInput);
			Vic2Armies.push_back(Vic2Army);
			theArmy.addSourceArmies(Vic2Armies);

			std::stringstream mappingsInput("= {\n"\
													  "\tmap = {\n"\
													  "\t\tlink = {\n"\
													  "\t\t\tvic = infantry\n"\
													  "\t\t\thoi = {\n"\
													  "\t\t\t\tcategory = land\n"\
													  "\t\t\t\ttype = infantry\n"\
													  "\t\t\t\tequipment = infantry_equipment_0\n"\
													  "\t\t\t\tsize = 3\n"\
													  "\t\t\t}\n"\
													  "\t\t}\n"\
													  "\t}\n"\
													  "\tdivision_templates = {\n"\
													  "\t\tdivision_template= {\n"\
													  "\t\t\tname = \"Infantry Brigade\"\n"\
													  "\t\t\tregiments = {\n"\
													  "\t\t\t\tinfantry = { x = 0 y = 0 }\n"\
													  "\t\t\t\tinfantry = { x = 1 y = 0 }\n"\
													  "\t\t\t}\n"\
													  "\t\t}\n"\
													  "\t\tdivision_template= {\n"\
													  "\t\t\tname = \"Light Infantry Brigade\"\n"\
													  "\t\t\tregiments = {\n"\
													  "\t\t\t\tinfantry = { x = 0 y = 0 }\n"\
													  "\t\t\t}\n"\
													  "\t\t}\n"\
													  "\t}\n"\
													  "}"
			);
			HoI4::militaryMappings theMilitaryMappings(std::string("default"), mappingsInput);
			theArmy.convertArmies(theMilitaryMappings, 0);

			std::ostringstream output;
			output << theArmy;
			Assert::AreEqual(std::string("\tdivision = {\n"\
												  "\t\tname = \"1. Infantry Brigade\"\n"\
												  "\t\tlocation = 11821\n"\
												  "\t\tdivision_template = \"Infantry Brigade\"\n"\
												  "\t\tstart_experience_factor = 0.3\n"\
												  "\t\tstart_equipment_factor = 0.7\n"\
												  "\t}\n"\
												  "\tdivision = {\n"\
												  "\t\tname = \"1. Light Infantry Brigade\"\n"\
												  "\t\tlocation = 11821\n"\
												  "\t\tdivision_template = \"Light Infantry Brigade\"\n"\
												  "\t\tstart_experience_factor = 0.3\n"\
												  "\t\tstart_equipment_factor = 0.7\n"\
												  "\t}\n"), output.str());

			delete Vic2Army;
		}
		TEST_METHOD(SubstituteDivisionsAllowConversion)
		{
			HoI4::Army theArmy;
			std::vector<const Vic2::Army*> Vic2Armies;
			std::istringstream armyInput(	"=\n"\
												  "\t{\n"\
												  "\t\tname=\"I. Legio\"\n"\
												  "\t\tlocation=496\n"\
												  "\t\tregiment=\n"\
												  "\t\t{\n"\
												  "\t\t\texperience=30.000\n"\
												  "\t\t\tcount=1\n"\
												  "\t\t\ttype=infantry\n"\
												  "\t\t}\n"\
												  "\t\tregiment=\n"\
												  "\t\t{\n"\
												  "\t\t\texperience=30.000\n"\
												  "\t\t\tcount=1\n"\
												  "\t\t\ttype=artillery\n"\
												  "\t\t}\n"\
												  "\t}");
			Vic2::Army* Vic2Army = new Vic2::Army("army", armyInput);
			Vic2Armies.push_back(Vic2Army);
			theArmy.addSourceArmies(Vic2Armies);

			std::stringstream mappingsInput("= {\n"\
													  "\tmap = {\n"\
													  "\t\tlink = {\n"\
													  "\t\t\tvic = infantry\n"\
													  "\t\t\thoi = {\n"\
													  "\t\t\t\tcategory = land\n"\
													  "\t\t\t\ttype = infantry\n"\
													  "\t\t\t\tequipment = infantry_equipment_0\n"\
													  "\t\t\t\tsize = 1\n"\
													  "\t\t\t}\n"\
													  "\t\t}\n"\
													  "\t\tlink = {\n"\
													  "\t\t\tvic = artillery\n"\
													  "\t\t\thoi = {\n"\
													  "\t\t\t\tcategory = land\n"\
													  "\t\t\t\ttype = artillery_brigade\n"\
													  "\t\t\t\tequipment = artillery_equipment_0\n"\
													  "\t\t\t\tsize = 1\n"\
													  "\t\t\t}\n"\
													  "\t\t}\n"\
													  "\t}\n"\
													  "\tdivision_templates = {\n"\
													  "\t\tdivision_template= {\n"\
													  "\t\t\tname = \"Light Infantry Brigade\"\n"\
													  "\t\t\tregiments = {\n"\
													  "\t\t\t\tinfantry = { x = 0 y = 0 }\n"\
													  "\t\t\t}\n"\
													  "\t\t\tsupport = {\n"\
													  "\t\t\t\tartillery = { x = 0 y = 0 }\n"\
													  "\t\t\t}\n"\
													  "\t\t}\n"\
													  "\t}\n"\
													  "\tsubstitutes = {\n"\
													  "\t\tartillery = artillery_brigade\n"\
													  "\t}\n"\
													  "}"
			);
			HoI4::militaryMappings theMilitaryMappings(std::string("default"), mappingsInput);
			theArmy.convertArmies(theMilitaryMappings, 0);

			std::ostringstream output;
			output << theArmy;
			Assert::AreEqual(std::string("\tdivision = {\n"\
												  "\t\tname = \"1. Light Infantry Brigade\"\n"\
												  "\t\tlocation = 11821\n"\
												  "\t\tdivision_template = \"Light Infantry Brigade\"\n"\
												  "\t\tstart_experience_factor = 0.3\n"\
												  "\t\tstart_equipment_factor = 0.7\n"\
												  "\t}\n"), output.str());

			delete Vic2Army;
		}
		TEST_METHOD(UnconvertedDivisionsMergeAndConvert)
		{
			HoI4::Army theArmy;
			std::vector<const Vic2::Army*> Vic2Armies;
			std::istringstream armyInput(	"=\n"\
												  "\t{\n"\
												  "\t\tname=\"I. Legio\"\n"\
												  "\t\tlocation=496\n"\
												  "\t\tregiment=\n"\
												  "\t\t{\n"\
												  "\t\t\texperience=30.000\n"\
												  "\t\t\tcount=1\n"\
												  "\t\t\ttype=infantry\n"\
												  "\t\t}\n"\
												  "\t}");
			Vic2::Army* Vic2Army = new Vic2::Army("army", armyInput);
			Vic2Armies.push_back(Vic2Army);
			std::istringstream armyInput2(	"=\n"\
												  "\t{\n"\
												  "\t\tname=\"I. Legio\"\n"\
												  "\t\tlocation=1496\n"\
												  "\t\tregiment=\n"\
												  "\t\t{\n"\
												  "\t\t\texperience=30.000\n"\
												  "\t\t\tcount=1\n"\
												  "\t\t\ttype=infantry\n"\
												  "\t\t}\n"\
												  "\t}");
			Vic2::Army* Vic2Army2 = new Vic2::Army("army", armyInput2);
			Vic2Armies.push_back(Vic2Army2);
			theArmy.addSourceArmies(Vic2Armies);

			std::stringstream mappingsInput("= {\n"\
													  "\tmap = {\n"\
													  "\t\tlink = {\n"\
													  "\t\t\tvic = infantry\n"\
													  "\t\t\thoi = {\n"\
													  "\t\t\t\tcategory = land\n"\
													  "\t\t\t\ttype = infantry\n"\
													  "\t\t\t\tequipment = infantry_equipment_0\n"\
													  "\t\t\t\tsize = 1\n"\
													  "\t\t\t}\n"\
													  "\t\t}\n"\
													  "\t}\n"\
													  "\tdivision_templates = {\n"\
													  "\t\tdivision_template= {\n"\
													  "\t\t\tname = \"Light Infantry Brigade\"\n"\
													  "\t\t\tregiments = {\n"\
													  "\t\t\t\tinfantry = { x = 0 y = 0 }\n"\
													  "\t\t\t\tinfantry = { x = 1 y = 0 }\n"\
													  "\t\t\t}\n"\
													  "\t\t}\n"\
													  "\t}\n"\
													  "}"
			);
			HoI4::militaryMappings theMilitaryMappings(std::string("default"), mappingsInput);
			theArmy.convertArmies(theMilitaryMappings, 1);

			std::ostringstream output;
			output << theArmy;
			Assert::AreEqual(std::string("\tdivision = {\n"\
												  "\t\tname = \"1. Light Infantry Brigade\"\n"\
												  "\t\tlocation = 1\n"\
												  "\t\tdivision_template = \"Light Infantry Brigade\"\n"\
												  "\t\tstart_experience_factor = 0.3\n"\
												  "\t\tstart_equipment_factor = 0.7\n"\
												  "\t}\n"), output.str());

			delete Vic2Army;
		}
};

}