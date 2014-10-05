#include "Configuration.h"
#include "Parsers/Parser.h"
#include "Log.h"

Configuration* Configuration::instance = NULL;

Configuration::Configuration()
{
	LOG(LogLevel::Info) << "Reading configuration file.";

	Object* tempObj = doParseFile("configuration.txt");
	if (tempObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file configuration.txt";
		exit(-1);
	}
	vector<Object*> obj = tempObj->getValue("configuration");
	if (obj.size() != 1)
	{
		LOG(LogLevel::Error) << "Configuration file must contain exactly one configuration section.";
		exit (-2);
	}

	maxLiteracy			= atof(obj[0]->getLeaf("max_literacy").c_str());
	resetProvinces		= obj[0]->getLeaf("resetProvinces");
	V2Path				= obj[0]->getLeaf("v2directory");
	V2DocumentsPath	= obj[0]->getLeaf("V2Documentsdirectory");
	EU3Path				= obj[0]->getLeaf("EU3directory");
	EU3gametype			= obj[0]->getLeaf("EU3gametype");
	V2gametype			= obj[0]->getLeaf("V2gametype");
	EU3Mod				= obj[0]->getLeaf("EU3Mod");
	removetype			= obj[0]->getLeaf("removetype");
	outputName			= "";
}