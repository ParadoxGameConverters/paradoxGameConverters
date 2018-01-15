/*Copyright (c) 2017 The Paradox Game Converters Project

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



Configuration* Configuration::instance = nullptr;



Configuration::Configuration():
	HoI4Path(""),
	HoI4DocumentsPath(""),
	V2Path(""),
	Vic2Mods(),
	outputName(""),
	forceMultiplier(0.0),
	manpowerFactor(0.0),
	industrialShapeFactor(0.0),
	icFactor(0.0),
	ideologiesOptions(ideologyOptions::keep_major),
	debug(false),
	removeCores(true),
	leaderID(1000),
	leaderIDCountryIdx(1),
	version()
{
	LOG(LogLevel::Info) << "Reading configuration file";

	auto oneObj = parser_UTF8::doParseFile("configuration.txt");	// the parsed configuration file
	if (oneObj)
	{
		auto obj = oneObj->safeGetObject("configuration");	// the configuration section
		if (obj == nullptr)
		{
			LOG(LogLevel::Error) << "Configuration missing configuration section";
			exit (-1);
		}

		V2Path = obj->safeGetString("V2directory");
		if (V2Path.empty() || !Utils::doesFolderExist(V2Path))
		{
			LOG(LogLevel::Error) << "No Victoria 2 path was specified in configuration.txt, or the path was invalid";
			exit(-1);
		}
		else if (!Utils::DoesFileExist(V2Path + "/v2game.exe"))
		{
			LOG(LogLevel::Error) << "The Victoria 2 path specified in configuration.txt does not contain Victoria 2";
			exit(-1);
		}
		else
		{
			LOG(LogLevel::Debug) << "Victoria 2 install path is " << V2Path;
		}

		HoI4Path = obj->safeGetString("HoI4directory");
		if (HoI4Path.empty() || !Utils::doesFolderExist(HoI4Path))
		{
			LOG(LogLevel::Error) << "No HoI4 path was specified in configuration.txt, or the path was invalid";
			exit(-1);
		}
		else if (!Utils::DoesFileExist(HoI4Path + "/hoi4.exe"))
		{
			LOG(LogLevel::Error) << "The HoI4 path specified in configuration.txt does not contain HoI4";
			exit(-1);
		}
		else
		{
			LOG(LogLevel::Debug) << "HoI4 path install path is " << HoI4Path;
		}

		HoI4DocumentsPath = obj->safeGetString("HoI4Documentsdirectory");
		if (HoI4DocumentsPath.empty() || !Utils::doesFolderExist(HoI4DocumentsPath))
		{
			LOG(LogLevel::Error) << "No HoI4 documents directory was specified in configuration.txt, or the path was invalid";
			exit(-1);
		}
		else
		{
			LOG(LogLevel::Debug) << "HoI4 documents directory is " << HoI4DocumentsPath;
		}

		Vic2Mods = obj->safeGetTokens("Vic2Mods");
		manpowerFactor = obj->safeGetFloat("manpower_factor");
		forceMultiplier = obj->safeGetFloat("force_multiplier");
		industrialShapeFactor = obj->safeGetFloat("industrial_shape_factor");
		icFactor = obj->safeGetFloat("ic_factor");

		string versionMethod = obj->safeGetString("HoI4VersionMethod");
		if (versionMethod == "automatic")
		{
			version = getAutomaticHoI4Version();
		}
		else if (versionMethod == "manualEntry")
		{
			version = HOI4Version(obj->safeGetString("HoI4Version"));
		}
		else // (versionMethod == "hardcoded")
		{
			version = HOI4Version();
		}
		Log(LogLevel::Debug) << "HoI4 version is " << version;

		string ideologyOptiongsString = obj->safeGetString("ideologies", "keep_major");
		if (ideologyOptiongsString == "keep_default")
		{
			ideologiesOptions = ideologyOptions::keep_default;
		}
		else if (ideologyOptiongsString == "keep_all")
		{
			ideologiesOptions = ideologyOptions::keep_all;
		}
		else // (ideologyOptiongsString == "keep_major")
		{
			ideologiesOptions = ideologyOptions::keep_major;
		}

		if (obj->safeGetString("debug") == "yes")
		{
			debug = true;
		}

		if (obj->safeGetString("remove_cores") == "false")
		{
			removeCores = false;
		}
	}
	else
	{
		LOG(LogLevel::Error) << "Could not open configuration.txt";
		exit(-1);
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

	LOG(LogLevel::Warning) << "Could not automatically set HoI4 version. Using the hardcoded version setting instead.";
	return HOI4Version();
}
