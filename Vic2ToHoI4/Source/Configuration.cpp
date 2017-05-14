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



#include "Configuration.h"
#include "OSCompatibilityLayer.h"
#include "ParadoxParserUTF8.h"
#include "Object.h"
#include "Log.h"
#include <fstream>
#include <vector>
using namespace std;



Configuration* Configuration::instance = NULL;

Configuration::Configuration()
{
	LOG(LogLevel::Info) << "Reading configuration file";

	Object* oneObj = parser_UTF8::doParseFile("configuration.txt");	// the parsed configuration file
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

	V2Path = obj[0]->getLeaf("V2directory");
	if (V2Path.empty() || !Utils::doesFolderExist(V2Path))
	{
		LOG(LogLevel::Error) << "No Victoria 2 path was specified in configuration.txt, or the path was invalid";
		exit(-1);
	}
	else
	{
		LOG(LogLevel::Debug) << "Victoria 2 install path is " << V2Path;
	}

	HoI4Path = obj[0]->getLeaf("HoI4directory");
	if (HoI4Path.empty() || !Utils::doesFolderExist(HoI4Path))
	{
		LOG(LogLevel::Error) << "No HoI4 path was specified in configuration.txt, or the path was invalid";
		exit(-1);
	}
	else
	{
		LOG(LogLevel::Debug) << "HoI4 path install path is " << HoI4Path;
	}

	HoI4DocumentsPath = obj[0]->getLeaf("HoI4Documentsdirectory");
	if (HoI4DocumentsPath.empty() || !Utils::doesFolderExist(HoI4DocumentsPath))
	{
		LOG(LogLevel::Error) << "No HoI4 documents directory was specified in configuration.txt, or the path was invalid";
		exit(-1);
	}
	else
	{
		LOG(LogLevel::Debug) << "HoI4 documents directory is " << HoI4DocumentsPath;
	}


	outputName = "";

	vector<Object*> modsObj = obj[0]->getValue("Vic2Mods");
	if (modsObj.size() > 0)
	{
		Vic2Mods = modsObj[0]->getTokens();
	}

	manpowerFactor			= stof(obj[0]->getLeaf("manpower_factor"));
	forceMultiplier			= stof(obj[0]->getLeaf("force_multiplier"));
	industrialShapeFactor= stof(obj[0]->getLeaf("industrial_shape_factor"));
	icFactor					= stof(obj[0]->getLeaf("ic_factor"));
	if (obj[0]->getValue("ICStats").size() > 0)
	{
		ICStats = true;
	}
	else
	{
		ICStats = false;
	}

	leaderID					= 1000;
	leaderIDCountryIdx	= 1;

	string versionMethod = obj[0]->getLeaf("HoI4VersionMethod");
	if (versionMethod == "automatic")
	{
		version = getAutomaticHoI4Version();
	}
	else if (versionMethod == "manualEntry")
	{
		version = HOI4Version(obj[0]->getLeaf("HoI4Version"));
	}
	else // (versionMethod == "hardcoded")
	{
		version = HOI4Version("1.3.3");
	}
}


HOI4Version Configuration::getAutomaticHoI4Version()
{
	ifstream systemLog(HoI4DocumentsPath + "/logs/system.log");
	if (systemLog.is_open())
	{
		while (!systemLog.eof())
		{
			char buffer[256];
			systemLog.getline(buffer, sizeof(buffer));
			string line(buffer);
			int versionPosition = line.find("Version: ");
			if (versionPosition != string::npos)
			{
				int position1 = line.find_first_of(' ', versionPosition);
				int position2 = line.find_first_of(' ', position1 + 1) + 2;
				int position3 = line.find_first_of(' ', position2 + 1);
				string versionString = line.substr(position2, position3 - position2);
				return HOI4Version(versionString);
			}
		}
	}

	LOG(LogLevel::Error) << "Could not automatically set HoI4 version. Run HoI4 and convert again, or use a different version setting.";
	exit(-1);

	return HOI4Version();
}
