/*Copyright (c) 2013 The CK2 to EU3 Converter Project
 
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


/*Copyright (c) 2010 The EU3 to V2 Converter Project
 
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
	printf("Reading configuration file.\n");
	log("Reading configuration file.\n");

	Object* oneObj = doParseFile("configuration.txt");
	if (oneObj == NULL)
	{
		log("Error: Could not open configuration.txt\n");
		printf("Error: Could not open configuration.txt\n");
		exit(-1);
	}

	vector<Object*> obj = oneObj->getValue("configuration");
	if (obj.size() != 1)
	{
		printf("Configuration file must contain exactly one configuration section.");
		log("Configuration file must contain exactly one configuration section.");
		exit (-2);
	}


	CK2Path		= obj[0]->getLeaf("CK2directory");
	EU3Path		= obj[0]->getLeaf("EU3directory");
	CK2ModPath	= obj[0]->getLeaf("CK2ModPath");
	modPath		= "";

	techGroupMethod	= obj[0]->getLeaf("techGroupMethod");

	proxyMultiplierMethod	= obj[0]->getLeaf("proxyMultiplierMethod");
	multipleProvsMethod		= obj[0]->getLeaf("multipleProvsMethod");
	manpower						= obj[0]->getLeaf("manpower");
	manpowerblendamount		= obj[0]->getLeaf("manpowerblendamount");
	basetax						= obj[0]->getLeaf("basetax");
	basetaxblendamount		= obj[0]->getLeaf("basetaxblendamount");
	population					= obj[0]->getLeaf("population");
	populationblendamount	= obj[0]->getLeaf("populationblendamount");
	HRETitle						= obj[0]->getLeaf("HRETitle");
	mergeTitles					= obj[0]->getLeaf("mergeTitles");
	vassalScore					= obj[0]->getLeaf("vassalScore");

	advisors				= obj[0]->getLeaf("advisors");
	leaders				= obj[0]->getLeaf("leaders");
	colonists			= obj[0]->getLeaf("colonists");
	merchants			= obj[0]->getLeaf("merchants");
	missionaries		= obj[0]->getLeaf("missionaries");
	inflation			= obj[0]->getLeaf("inflation");
	colonist_size		= obj[0]->getLeaf("colonist_size");
	difficulty			= obj[0]->getLeaf("difficulty");
	AI_aggressiveness	= obj[0]->getLeaf("AI_aggressiveness");
	land_spread			= obj[0]->getLeaf("land_spread");
	sea_spread			= obj[0]->getLeaf("sea_spread");
	spies					= obj[0]->getLeaf("spies");
	lucky_nations		= obj[0]->getLeaf("lucky_nations");

	useConverterMod	= obj[0]->getLeaf("useConverterMod");
	CK2Mod				= obj[0]->getLeaf("CK2Mod");

	id				= 1;
	armyId		= 1;
}