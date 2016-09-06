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



#include "V2Factory.h"
#include "ParadoxParser8859_15.h"
#include "Log.h"
#include "../Configuration.h"



V2FactoryType::V2FactoryType(Object* factory)
{
	name = factory->getKey();

	vector<Object*> is_coastal = factory->getValue("is_coastal");
	if ((is_coastal.size() > 0) && (is_coastal[0]->getLeaf() == "yes"))
	{
		requireCoastal = true;
	}
	else
	{
		requireCoastal = false;
	}

	requireTech						= "";
	vanillaRequiredInvention	= (vanillaInventionType)-1;
	HODRequiredInvention			= (HODInventionType)-1;
	HODNNMRequiredInvention		= (HODNNMInventionType)-1;

	vector<Object*> local_supply = factory->getValue("limit_by_local_supply");
	if ((local_supply.size() > 0) && (local_supply[0]->getLeaf() == "yes"))
	{
		requireLocalInput = true;
	}
	else
	{
		requireLocalInput = false;
	}

	inputs.clear();
	vector<Object*> inputGoods = factory->getValue("input_goods");
	if (inputGoods.size() > 0)
	{
		vector<Object*> inObjs = inputGoods[0]->getLeaves();
		for (vector<Object*>::iterator itr = inObjs.begin(); itr != inObjs.end(); ++itr)
		{
			inputs.insert(make_pair((*itr)->getKey(), (float)atof((*itr)->getLeaf().c_str())));
		}
	}

	vector<Object*> outputGoodsObj = factory->getValue("output_goods");
	if (outputGoodsObj.size() > 0)
	{
		outputGoods = outputGoodsObj[0]->getLeaf();
	}
}


void V2Factory::output(FILE* output) const
{
	// V2 takes care of hiring employees on day 1, provided sufficient starting capital
	fprintf(output, "state_building=\n");
	fprintf(output, "{\n");
	fprintf(output, "\tlevel=%d\n", level);
	fprintf(output, "\tbuilding = %s\n", type->name.c_str());
	fprintf(output, "\tupgrade = yes\n");
	fprintf(output, "}\n");
}


map<string,float> V2Factory::getRequiredRGO() const
{
	if (type->requireLocalInput)
	{
		return type->inputs;
	}
	else
	{
		map<string,float> emptyMap;
		emptyMap.clear();
		return emptyMap;
	}
}


void V2Factory::increaseLevel()
{
	level++;
}


V2FactoryFactory::V2FactoryFactory()
{
	// load required techs/inventions
	factoryTechReqs.clear();
	loadRequiredTechs(Configuration::getV2Path() + "\\technologies\\army_tech.txt");
	loadRequiredTechs(Configuration::getV2Path() + "\\technologies\\commerce_tech.txt");
	loadRequiredTechs(Configuration::getV2Path() + "\\technologies\\culture_tech.txt");
	loadRequiredTechs(Configuration::getV2Path() + "\\technologies\\industry_tech.txt");
	loadRequiredTechs(Configuration::getV2Path() + "\\technologies\\navy_tech.txt");
	factoryInventionReqs.clear();
	loadRequiredInventions(Configuration::getV2Path() + "\\inventions\\army_inventions.txt");
	loadRequiredInventions(Configuration::getV2Path() + "\\inventions\\commerce_inventions.txt");
	loadRequiredInventions(Configuration::getV2Path() + "\\inventions\\culture_inventions.txt");
	loadRequiredInventions(Configuration::getV2Path() + "\\inventions\\industry_inventions.txt");
	loadRequiredInventions(Configuration::getV2Path() + "\\inventions\\navy_inventions.txt");

	// load factory types
	factoryTypes.clear();
	Object* obj = parser_8859_15::doParseFile((Configuration::getV2Path() + "\\common\\production_types.txt").c_str());
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << Configuration::getV2Path() << "\\common\\production_types.txt";
		exit(-1);
	}
	vector<Object*> factoryObjs = obj->getLeaves();
	for (vector<Object*>::iterator itr = factoryObjs.begin(); itr != factoryObjs.end(); ++itr)
	{
		V2FactoryType* ft = new V2FactoryType(*itr);
		map<string,string>::iterator reqitr = factoryTechReqs.find(ft->name);
		if (reqitr != factoryTechReqs.end())
		{
			ft->requireTech = reqitr->second;
		}
		reqitr = factoryInventionReqs.find(ft->name);
		if (reqitr != factoryInventionReqs.end())
		{
			for (int i = 0; i <= VANILLA_naval_exercises; ++i)
			{
				if (((Configuration::getV2Gametype() == "vanilla") || (Configuration::getV2Gametype() == "AHD")) && (reqitr->second == vanillaInventionNames[i]))
				{
					ft->vanillaRequiredInvention = (vanillaInventionType)i;
					break;
				}
				else if ((Configuration::getV2Gametype() == "HOD") && (reqitr->second == HODInventionNames[i]))
				{
					ft->HODRequiredInvention = (HODInventionType)i;
					break;
				}
				else if ((Configuration::getV2Gametype() == "HoD-NNM") && (reqitr->second == HODNNMInventionNames[i]))
				{
					ft->HODNNMRequiredInvention = (HODNNMInventionType)i;
					break;
				}
			}
		}
		factoryTypes[ft->name] = ft;
	}

	factoryCounts.clear();
	obj = parser_8859_15::doParseFile("starting_factories.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file starting_factories.txt";
		exit(-1);
	}
	vector<Object*> top = obj->getValue("starting_factories");
	if (top.size() != 1)
	{
		LOG(LogLevel::Error) << "Error: Could not load starting factory list!";
		printf("Error: Could not load starting factory list!\n");
		exit(-1);
	}
	vector<Object*> factories = top[0]->getLeaves();
	for (vector<Object*>::iterator itr = factories.begin(); itr != factories.end(); ++itr)
	{
		string factoryType = (*itr)->getKey();
		int count = atoi((*itr)->getLeaf().c_str());

		map<string, V2FactoryType*>::iterator t = factoryTypes.find(factoryType);
		if (t == factoryTypes.end())
		{
			LOG(LogLevel::Error) << "Error: Could not locate V2 factory type for starting factories of type %s!";
			continue;
		}
		factoryCounts.push_back(pair<V2FactoryType*, int>(t->second, count));
	}
}


deque<V2Factory*> V2FactoryFactory::buildFactories() const
{
	deque<V2Factory*> retval;
	for (vector< pair<V2FactoryType*, int> >::const_iterator itr = factoryCounts.begin(); itr != factoryCounts.end(); ++itr)
	{
		for (int i = 0; i < itr->second; ++i)
		{
			V2Factory* newFactory = new V2Factory(itr->first);
			retval.push_back(newFactory);
		}
	}
	return retval;
}


void V2FactoryFactory::loadRequiredTechs(string filename)
{
	Object* obj = parser_8859_15::doParseFile(filename.c_str());
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << filename;
		exit(-1);
	}
	vector<Object*> techObjs = obj->getLeaves();
	for (vector<Object*>::iterator itr = techObjs.begin(); itr != techObjs.end(); ++itr)
	{
		vector<Object*> building = (*itr)->getValue("activate_building");
		for (vector<Object*>::iterator bitr = building.begin(); bitr != building.end(); ++bitr)
		{
			factoryTechReqs.insert(make_pair((*bitr)->getLeaf(), (*itr)->getKey()));
		}
	}
}


void V2FactoryFactory::loadRequiredInventions(string filename)
{
	Object* obj = parser_8859_15::doParseFile(filename.c_str());
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << filename;
		exit(-1);
	}
	vector<Object*> invObjs = obj->getLeaves();
	for (vector<Object*>::iterator itr = invObjs.begin(); itr != invObjs.end(); ++itr)
	{
		vector<Object*> effect = (*itr)->getValue("effect");
		if (effect.size() == 0)
		{
			continue;
		}
		vector<Object*> building = effect[0]->getValue("activate_building");
		for (vector<Object*>::iterator bitr = building.begin(); bitr != building.end(); ++bitr)
		{
			factoryInventionReqs.insert(make_pair((*bitr)->getLeaf(), (*itr)->getKey()));
		}
	}
}