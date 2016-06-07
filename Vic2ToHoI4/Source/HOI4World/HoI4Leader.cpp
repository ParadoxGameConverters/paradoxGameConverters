/*Copyright (c) 2016 The Paradox Game Converters Project

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


#include "HoI4Leader.h"
#include "..\Configuration.h"



HoI4Leader::HoI4Leader(vector<wstring>& firstNames, vector<wstring>& lastNames, wstring _country, wstring _type, leaderTraitsMap& _traitsMap, vector<wstring>& portraits)
{
	ID			= Configuration::getNextLeaderID();
	name		= firstNames[rand() % firstNames.size()] + L" " + lastNames[rand() % lastNames.size()];
	country	= _country;
	type		= _type;
	skill		= 0;
	rank		= 1;
	//picture	= portraits[rand() % portraits.size()];

	//if (rand() % 4 > 0)
	//{
	//	vector<wstring> allTraits = _traitsMap.find(_type)->second;
	//	traits.push_back(allTraits[rand() % allTraits.size()]);

	//	if (rand() % 10 > 8)
	//	{
	//		wstring secondTrait = allTraits[rand() % allTraits.size()];
	//		while (traits[0] == secondTrait)
	//		{
	//			secondTrait = allTraits[rand() % allTraits.size()];
	//		}
	//		traits.push_back(secondTrait);
	//	}
	//}
}


HoI4Leader::HoI4Leader(V2Leader* srcLeader, wstring _country, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap, vector<wstring>& portraits)
{
	ID			= Configuration::getNextLeaderID();
	name		= srcLeader->getName();
	country	= _country;
	if (srcLeader->getType() == L"land")
	{
		type = L"land";
	}
	else if (srcLeader->getType() == L"sea")
	{
		type = L"sea";
	}
	skill		= static_cast<int>(srcLeader->getPrestige() * 22.5f);
	if (skill > 9)
	{
		skill = 9;
	}
	rank		= 4;
	//picture	= portraits[rand() % portraits.size()];

	if (type == L"land")
	{
		auto possiblePersonalities = landPersonalityMap.find(srcLeader->getPersonality());
		if ((possiblePersonalities != landPersonalityMap.end()) && (possiblePersonalities->second.size() > 0))
		{
			traits.push_back(possiblePersonalities->second[rand() % possiblePersonalities->second.size()]);
		}
		auto possibleBackgrounds = landBackgroundMap.find(srcLeader->getBackground());
		if ((possibleBackgrounds != landBackgroundMap.end()) && (possibleBackgrounds->second.size() > 0))
		{
			traits.push_back(possibleBackgrounds->second[rand() % possibleBackgrounds->second.size()]);
		}
	}
	else if (type == L"sea")
	{
		auto possiblePersonalities = seaPersonalityMap.find(srcLeader->getPersonality());
		if ((possiblePersonalities != seaPersonalityMap.end()) && (possiblePersonalities->second.size() > 0))
		{
			traits.push_back(possiblePersonalities->second[rand() % possiblePersonalities->second.size()]);
		}
		auto possibleBackgrounds = seaBackgroundMap.find(srcLeader->getBackground());
		if ((possibleBackgrounds != seaBackgroundMap.end()) && (possibleBackgrounds->second.size() > 0))
		{
			traits.push_back(possibleBackgrounds->second[rand() % possibleBackgrounds->second.size()]);
		}
	}
}


void HoI4Leader::output(FILE * output)
{
	fwprintf(output, L"%d = {\n", ID);
	fwprintf(output, L"\tname = \"%s\"\n", name.c_str());
	fwprintf(output, L"\tcountry = %s\n", country.c_str());
	fwprintf(output, L"\ttype = %s\n", type.c_str());
	fwprintf(output, L"\tskill = %d", skill);
	fwprintf(output, L"\tmax_skill = 9\n");
	fwprintf(output, L"\tloyalty = 1.00\n");
	fwprintf(output, L"\tpicture = %s\n", picture.c_str());
	for (auto trait: traits)
	{
		fwprintf(output, L"\tadd_trait = %s\n", trait.c_str());
	}
	fwprintf(output, L"\thistory = {\n");
	fwprintf(output, L"\t\t1936.1.1 = { rank = %d }\n", rank);
	fwprintf(output, L"\t\t1970.1.1 = { rank = 0 }\n");
	fwprintf(output, L"\t}\n");
	fwprintf(output, L"}\n");
}