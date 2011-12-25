#include "Configuration.h"
#include "Parsers/Parser.h"
#include "Log.h"

Configuration* Configuration::instance = NULL;

Configuration::Configuration()
{
	printf("Reading configuration file.\n");
	log("Reading configuration file.\n");

	vector<Object*> obj = doParseFile("configuration.txt")->getValue("configuration");
	if (obj.size() != 1)
	{
		printf("Configuration file must contain exactly one configuration section.");
		log("Configuration file must contain exactly one configuration section.");
		exit (-2);
	}

	V2Path = obj[0]->getLeaf("v2directory");
	startDate = date(obj[0]->getLeaf("start_date"));
	gametype = obj[0]->getLeaf("gametype");
}