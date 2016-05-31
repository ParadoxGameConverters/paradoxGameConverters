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
#include "Parsers\Parser.h"
#include "Log.h"
#include <vector>
using namespace std;



Configuration* Configuration::instance = NULL;

Configuration::Configuration()
{
	LOG(Info) << "Reading configuration file.\n";

	Object* oneObj = doParseFile("configuration.txt");
	if (oneObj == NULL)
	{
		LOG(Error) << "Error: Could not open configuration.txt\n";
		exit(-1);
	}

	vector<IObject*> obj = oneObj->getValue("configuration");
	if (obj.size() != 1)
	{
		LOG(Error) << "Configuration file must contain exactly one configuration section.\n";
		exit (-2);
	}

	readConfiguration(obj[0]);
}

Configuration::Configuration(IObject* configurationDetails)
{
	readConfiguration(configurationDetails);
}

void Configuration::readConfiguration(IObject* obj)
{
	CK2Path		= obj->getLeaf("CK2directory");
	EU3Path		= obj->getLeaf("EU3directory");
	CK2ModPath	= obj->getLeaf("CK2ModPath");
	modPath		= "";

	techGroupMethod	= obj->getLeaf("techGroupMethod");

	proxyMultiplierMethod	= obj->getLeaf("proxyMultiplierMethod");
	multipleProvsMethod		= obj->getLeaf("multipleProvsMethod");
	manpower						= obj->getLeaf("manpower");
	manpowerblendamount		= obj->getLeaf("manpowerblendamount");
	basetax						= obj->getLeaf("basetax");
	basetaxblendamount		= obj->getLeaf("basetaxblendamount");
	population					= obj->getLeaf("population");
	populationblendamount	= obj->getLeaf("populationblendamount");
	HRETitle						= obj->getLeaf("HRETitle");
	mergeTitles					= obj->getLeaf("mergeTitles");
	vassalScore					= obj->getLeaf("vassalScore");

	advisors				= obj->getLeaf("advisors");
	leaders				= obj->getLeaf("leaders");
	colonists			= obj->getLeaf("colonists");
	merchants			= obj->getLeaf("merchants");
	missionaries		= obj->getLeaf("missionaries");
	inflation			= obj->getLeaf("inflation");
	colonist_size		= obj->getLeaf("colonist_size");
	difficulty			= obj->getLeaf("difficulty");
	AI_aggressiveness	= obj->getLeaf("AI_aggressiveness");
	land_spread			= obj->getLeaf("land_spread");
	sea_spread			= obj->getLeaf("sea_spread");
	spies					= obj->getLeaf("spies");
	lucky_nations		= obj->getLeaf("lucky_nations");

	useConverterMod	= obj->getLeaf("useConverterMod");
	CK2Mod				= obj->getLeaf("CK2Mod");

	id				= 1;
	armyId		= 1;
}
