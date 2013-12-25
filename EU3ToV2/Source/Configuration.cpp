#include "Configuration.h"
#include "Parsers/Parser.h"
#include "Log.h"

Configuration* Configuration::instance = NULL;

Configuration::Configuration()
{
	printf("Reading configuration file.\n");
	log("Reading configuration file.\n");

	Object* tempObj = doParseFile("configuration.txt");
	if (tempObj == NULL)
	{
		log("Could not parse file configuration.txt\n");
		exit(-1);
	}
	vector<Object*> obj = tempObj->getValue("configuration");
	if (obj.size() != 1)
	{
		printf("Configuration file must contain exactly one configuration section.");
		log("Configuration file must contain exactly one configuration section.");
		exit (-2);
	}

	startDate	= date(obj[0]->getLeaf("start_date"));
	if (startDate < date("1835.1.1"))
	{
		log("Warning: start dates prior to 1835 are likely to cause crashes!\n");
	}
	maxLiteracy	= atof(obj[0]->getLeaf("max_literacy").c_str());
	V2Path		= obj[0]->getLeaf("v2directory");
	EU3Path		= obj[0]->getLeaf("EU3directory");
	EU3gametype	= obj[0]->getLeaf("EU3gametype");
	V2gametype	= obj[0]->getLeaf("V2gametype");
	removetype	= obj[0]->getLeaf("removetype");
}