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



#include "Configuration.h"
#include "OSCompatibilityLayer.h"
#include "Log.h"
#include "ParserHelpers.h"
#include <fstream>
#include <vector>




Configuration theConfiguration;



void Configuration::instantiate(std::istream& theStream)
{
	registerKeyword(std::regex("HoI4directory"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString directoryString(theStream);
		HoI4Path = directoryString.getString();
		if (HoI4Path.empty() || !Utils::doesFolderExist(HoI4Path))
		{
			LOG(LogLevel::Error) << "No HoI4 path was specified in configuration.txt, or the path was invalid";
			std::exit(EXIT_FAILURE);
		}
		else if (!Utils::DoesFileExist(HoI4Path + "/hoi4.exe"))
		{
			LOG(LogLevel::Error) << "The HoI4 path specified in configuration.txt does not contain HoI4";
			std::exit(EXIT_FAILURE);
		}
		else
		{
			LOG(LogLevel::Debug) << "HoI4 path install path is " << HoI4Path;
		}
	});
	registerKeyword(std::regex("HoI4Documentsdirectory"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString directoryString(theStream);
		HoI4DocumentsPath = directoryString.getString();
		if (HoI4DocumentsPath.empty() || !Utils::doesFolderExist(HoI4DocumentsPath))
		{
			LOG(LogLevel::Error) << "No HoI4 documents directory was specified in configuration.txt, or the path was invalid";
			std::exit(EXIT_FAILURE);
		}
		else
		{
			LOG(LogLevel::Debug) << "HoI4 documents directory is " << HoI4DocumentsPath;
		}
	});
	registerKeyword(std::regex("V2directory"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString directoryString(theStream);
		Vic2Path = directoryString.getString();
		if (Vic2Path.empty() || !Utils::doesFolderExist(Vic2Path))
		{
			LOG(LogLevel::Error) << "No Victoria 2 path was specified in configuration.txt, or the path was invalid";
			std::exit(EXIT_FAILURE);
		}
		else if (!Utils::DoesFileExist(Vic2Path + "/v2game.exe"))
		{
			LOG(LogLevel::Error) << "The Victoria 2 path specified in configuration.txt does not contain Victoria 2";
			std::exit(EXIT_FAILURE);
		}
		else
		{
			LOG(LogLevel::Debug) << "Victoria 2 install path is " << Vic2Path;
		}
	});
	registerKeyword(std::regex("Vic2Mods"), [this](const std::string& unused, std::istream& theStream){
		commonItems::stringList modsStrings(theStream);
		Vic2Mods = modsStrings.getStrings();
	});
	registerKeyword(std::regex("force_multiplier"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleDouble factorValue(theStream);
		forceMultiplier = factorValue.getDouble();
	});
	registerKeyword(std::regex("manpower_factor"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleDouble factorValue(theStream);
		manpowerFactor = factorValue.getDouble();
	});
	registerKeyword(std::regex("industrial_shape_factor"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleDouble factorValue(theStream);
		forceMultiplier = factorValue.getDouble();
	});
	registerKeyword(std::regex("ic_factor"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleDouble factorValue(theStream);
		icFactor = factorValue.getDouble();
	});
	registerKeyword(std::regex("ideologies"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString ideologiesOptionString(theStream);
		if (ideologiesOptionString.getString() == "keep_default")
		{
			ideologiesOptions = ideologyOptions::keep_default;
		}
		else if (ideologiesOptionString.getString() == "keep_all")
		{
			ideologiesOptions = ideologyOptions::keep_all;
		}
		else if (ideologiesOptionString.getString() == "specified")
		{
			ideologiesOptions = ideologyOptions::specified;
		}
		else // (ideologiesOptionString.getString() == "keep_major")
		{
			ideologiesOptions = ideologyOptions::keep_major;
		}
	});
	registerKeyword(std::regex("ideologies_choice"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString choiceString(theStream);
		specifiedIdeologies.push_back(choiceString.getString());
	});
	registerKeyword(std::regex("debug"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString debugString(theStream);
		if (debugString.getString() == "yes")
		{
			debug = true;
		}
	});
	registerKeyword(std::regex("remove_cores"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString removecoreString(theStream);
		if (removecoreString.getString() == "false")
		{
			removeCores = false;
		}
	});
	registerKeyword(std::regex("create_factions"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString createFactionsString(theStream);
		if (createFactionsString.getString() == "no")
		{
			createFactions = false;
		}
	});
	std::string versionMethod;
	registerKeyword(std::regex("HoI4VersionMethod"), [&versionMethod](const std::string& unused, std::istream& theStream){
		commonItems::singleString versionMethodString(theStream);
		versionMethod = versionMethodString.getString();
	});
	std::string manualVersionString;
	registerKeyword(std::regex("HoI4Version"), [&manualVersionString](const std::string& unused, std::istream& theStream){
		commonItems::singleString versionString(theStream);
		manualVersionString = versionString.getString();
	});

	LOG(LogLevel::Info) << "Reading configuration file";
	parseStream(theStream);

	if (versionMethod == "automatic")
	{
		version = getAutomaticHoI4Version();
	}
	else if (versionMethod == "manualEntry")
	{
		version = HoI4::Version(manualVersionString);
	}
	else // (versionMethod == "hardcoded")
	{
		version = HoI4::Version();
	}
	Log(LogLevel::Debug) << "HoI4 version is " << version;
}


HoI4::Version Configuration::getAutomaticHoI4Version()
{
	std::ifstream systemLog(HoI4DocumentsPath + "/logs/system.log");
	if (systemLog.is_open())
	{
		while (!systemLog.eof())
		{
			char buffer[256];
			systemLog.getline(buffer, sizeof(buffer));
			std::string line(buffer);
			int versionPosition = line.find("Version: ");
			if (versionPosition != std::string::npos)
			{
				int position1 = line.find_first_of(' ', versionPosition);
				int position2 = line.find_first_of(' ', position1 + 1) + 2;
				int position3 = line.find_first_of(' ', position2 + 1);
				std::string versionString = line.substr(position2, position3 - position2);
				return HoI4::Version(versionString);
			}
		}
	}

	LOG(LogLevel::Warning) << "Could not automatically set HoI4 version. Using the hardcoded version setting instead.";
	return HoI4::Version();
}


ConfigurationFile::ConfigurationFile(const std::string& filename)
{
	registerKeyword(std::regex("configuration"), [](const std::string& unused, std::istream& theStream){
		theConfiguration.instantiate(theStream);
	});

	parseFile(filename);
}
