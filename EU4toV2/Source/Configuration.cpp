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

	Object* oneObj = doParseFile("configuration.txt");
	if (oneObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not open configuration.txt";
		exit(-1);
	}

	vector<Object*> obj = oneObj->getValue("configuration");
	if (obj.size() != 1)
	{
		LOG(LogLevel::Error) << "Configuration file must contain exactly one configuration section";
		exit (-2);
	}

	EU4Path				= obj[0]->getLeaf("EU4directory");
	EU4ModPath			= obj[0]->getLeaf("EU4ModDirectory");
	CK2ExportPath		= obj[0]->getLeaf("CK2ExportDirectory");
	V2Path				= obj[0]->getLeaf("V2directory");
	V2DocumentsPath	= obj[0]->getLeaf("V2Documentsdirectory");
	V2Gametype			= obj[0]->getLeaf("V2gametype");
	CK2Converted		= (obj[0]->getLeaf("CK2Converted") == "yes");
	resetProvinces		= obj[0]->getLeaf("resetProvinces");
	MaxLiteracy			= atof(obj[0]->getLeaf("max_literacy").c_str());
	Removetype			= obj[0]->getLeaf("Removetype");
	outputName			= "";
}