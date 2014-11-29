/*Copyright (c) 2014 The Paradox Game Converters Project

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



#include "Configuration.h"
#include "Parsers/Parser.h"
#include "Parsers\Object.h"
#include "Log.h"
#include <vector>
using namespace std;



Configuration* Configuration::instance = NULL;

Configuration::Configuration()
{
	LOG(LogLevel::Info) << "Reading configuration file";

	Object* oneObj = doParseFile("configuration.txt");	// the parsed configuration file
	if (oneObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not open configuration.txt";
		exit(-1);
	}

	vector<Object*> obj = oneObj->getValue("configuration");	// the configuration section
	if (obj.size() != 1)
	{
		LOG(LogLevel::Error) << "Configuration file must contain exactly one configuration section";
		exit (-1);
	}

	//EU4Path				= obj[0]->getLeaf("EU4directory");
	//EU4DocumentsPath	= obj[0]->getLeaf("EU4DocumentsDirectory");
	//CK2ExportPath		= obj[0]->getLeaf("CK2ExportDirectory");
	V2Path				= obj[0]->getLeaf("V2directory");
	V2DocumentsPath	= obj[0]->getLeaf("V2Documentsdirectory");
	V2Gametype = obj[0]->getLeaf("V2gametype");
	HoI3Path = obj[0]->getLeaf("HoI3directory");
	HoI3DocumentsPath = obj[0]->getLeaf("HoI3Documentsdirectory");
	HoI3Gametype = obj[0]->getLeaf("HoI3gametype");
	//resetProvinces		= obj[0]->getLeaf("resetProvinces");
	//MaxLiteracy			= atof(obj[0]->getLeaf("max_literacy").c_str());
	Removetype			= obj[0]->getLeaf("Removetype");
	outputName = "";

	moneyFactor = atof(obj[0]->getLeaf("money_factor").c_str());
	factionLeaderAlgorithm = obj[0]->getLeaf("faction_leader");
	minInfra = atof(obj[0]->getLeaf("min_infrastructure").c_str());
	if (factionLeaderAlgorithm == "manual")
	{
		vector<Object*> factionObj = obj[0]->getValue("axis_faction");
		if (factionObj.size() > 0)
		{
			manualAxisFaction = factionObj[0]->getTokens();
		}
		factionObj = obj[0]->getValue("allies_faction");
		if (factionObj.size() > 0)
		{
			manualAlliesFaction = factionObj[0]->getTokens();
		}
		factionObj = obj[0]->getValue("comintern_faction");
		if (factionObj.size() > 0)
		{
			manualCominternFaction = factionObj[0]->getTokens();
		}
	}

	icConversion = obj[0]->getLeaf("ic_conversion");
	icFactor = atof(obj[0]->getLeaf("ic_factor").c_str());
	manpowerConversion = obj[0]->getLeaf("manpower_conversion");
	manpowerFactor = atof(obj[0]->getLeaf("manpower_factor").c_str());
	leadershipConversion = obj[0]->getLeaf("leadership_conversion");
	leadershipFactor = atof(obj[0]->getLeaf("leadership_factor").c_str());
	literacyWeight = atof(obj[0]->getLeaf("literacy_weight").c_str());
	practicalsScale = atof(obj[0]->getLeaf("practicals_scale").c_str());
}