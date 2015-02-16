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



#include "V2Province.h"
#include "../Log.h"
#include "../Parsers\Object.h"
#include "../Parsers\Parser.h"
#include "../EU4World/EU4World.h"
#include "../EU4World/EU4Province.h"
#include "V2Pop.h"
#include "V2Country.h"
#include "V2Factory.h"
#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <sys/stat.h>
using namespace std;



V2Province::V2Province(string _filename)
{
	srcProvince			= NULL;
	filename				= _filename;
	coastal				= false;
	num					= 0;
	name					= "";
	ownerString = "";
	//controler			= "";
	cores.clear();
	colonyLevel			= 0;
	colonial				= 0;
	colonised			= false;
	landConnection		= false;
	sameContinent		= false;
	originallyInfidel	= false;
	oldPopulation		= 0;
	demographics.clear();
	oldPops.clear();
	pops.clear();
	rgoType				= "";
	terrain				= "";
	lifeRating			= 0;
	slaveState			= false;

	for (int i = 0; i < num_reg_categories; ++i)
	{
		unitNameCount[i] = 0;
	}
	
	fortLevel			= 0;
	navalBaseLevel		= 0;
	railLevel			= 0;
	factories.clear();

	int slash		= filename.find_last_of("\\");
	int numDigits	= filename.find_first_of("-") - slash - 2;
	string temp		= filename.substr(slash + 1, numDigits);
	num				= atoi(temp.c_str());

	Object* obj;
	struct _stat st;
	if (_stat((string(".\\blankMod\\output\\history\\provinces") + _filename).c_str(), &st) == 0)
	{
		LOG(LogLevel::Debug) << "Parsing " << (string(".\\blankMod\\output\\history\\provinces") + _filename);
		obj = doParseFile((string(".\\blankMod\\output\\history\\provinces") + _filename).c_str());
		if (obj == NULL)
		{
			LOG(LogLevel::Error) << "Could not parse .\\blankMod\\output\\history\\provinces" << _filename;
			exit(-1);
		}
	}
	else
	{
		LOG(LogLevel::Debug) << "Parsing " << (Configuration::getV2Path() + "\\history\\provinces" + _filename);
		obj = doParseFile((Configuration::getV2Path() + "\\history\\provinces" + _filename).c_str());
		if (obj == NULL)
		{
			LOG(LogLevel::Error) << "Could not parse " << Configuration::getV2Path() << "\\history\\provinces" << _filename;
			exit(-1);
		}
	}
	vector<Object*> leaves = obj->getLeaves();
	for (vector<Object*>::iterator itr = leaves.begin(); itr != leaves.end(); itr++)
	{
		if ((*itr)->getKey() == "owner")
		{
			ownerString = (*itr)->getLeaf();
		}
		else if ((*itr)->getKey() == "controller")
		{
			//controller = (*itr)->getLeaf().c_str();
		}
		else if ((*itr)->getKey() == "add_core")
		{
			cores.push_back((*itr)->getLeaf());
		}
		else if ((*itr)->getKey() == "trade_goods")
		{
			rgoType = (*itr)->getLeaf();
		}
		else if ((*itr)->getKey() == "life_rating")
		{
			lifeRating = atoi((*itr)->getLeaf().c_str());
		}
		else if ((*itr)->getKey() == "terrain")
		{
			terrain = (*itr)->getLeaf();
		}
		else if ((*itr)->getKey() == "colonial")
		{
			colonial = atoi((*itr)->getLeaf().c_str());
		}
		else if ((*itr)->getKey() == "colony")
		{
			colonyLevel = atoi((*itr)->getLeaf().c_str());
		}
		else if ((*itr)->getKey() == "naval_base")
		{
			navalBaseLevel = atoi((*itr)->getLeaf().c_str());
		}
		else if ((*itr)->getKey() == "fort")
		{
			fortLevel = atoi((*itr)->getLeaf().c_str());
		}
		else if ((*itr)->getKey() == "railroad")
		{
			railLevel = atoi((*itr)->getLeaf().c_str());
		}
		else if ((*itr)->getKey() == "is_slave")
		{
			if ((*itr)->getLeaf() == "yes")
			{
				slaveState = true;
			}
		}
		else if ((*itr)->getKey() == "state_building")
		{
			//LOG(LogLevel::Info) << "There's a building here, I promise...";
			cout << (*itr)->getLeaf().c_str();
		}
		else if ((*itr)->getKey() == "party_loyalty")
		{
		}
		else
		{
			//log("Unknown key - %s\n", (*itr)->getKey().c_str());
		}
	}
}

V2Province::V2Province(Object* obj) {
	num = 0 - atoi(obj->getKey().c_str());

	vector<Object*> ownerObjs;				// the object holding the owner
	ownerObjs = obj->getValue("owner");
	(ownerObjs.size() == 0) ? ownerString = "" : ownerString = ownerObjs[0]->getLeaf();
	owner = NULL;

	cores.clear();
	vector<Object*> coreObjs;				// the object holding the cores
	coreObjs = obj->getValue("core");
	for (unsigned int i = 0; i < coreObjs.size(); i++)
	{
		cores.push_back(coreObjs[i]->getLeaf());
	}

	vector<Object*> buildingObjs;
	fortLevel = 0;
	buildingObjs = obj->getValue("fort");
	if (buildingObjs.size() > 0)
	{
		vector<string> tokens = buildingObjs[0]->getTokens();
		if (tokens.size() > 0)
		{
			fortLevel = atoi(tokens[0].c_str());
		}
	}
	navalBaseLevel = 0;
	buildingObjs = obj->getValue("naval_base");
	if (buildingObjs.size() > 0)
	{
		vector<string> tokens = buildingObjs[0]->getTokens();
		if (tokens.size() > 0)
		{
			navalBaseLevel = atoi(tokens[0].c_str());
		}
	}
	railLevel = 0;
	buildingObjs = obj->getValue("railroad");
	if (buildingObjs.size() > 0)
	{
		vector<string> tokens = buildingObjs[0]->getTokens();
		if (tokens.size() > 0)
		{
			railLevel = atoi(tokens[0].c_str());
		}
	}

	// read pops
	vector<Object*> children = obj->getLeaves();
	for (vector<Object*>::iterator itr = children.begin(); itr != children.end(); ++itr)
	{
		string key = (*itr)->getKey();
		if (key == "aristocrats" || key == "artisans" || key == "bureaucrats" || key == "capitalists" || key == "clergymen"
			|| key == "craftsmen" || key == "clerks" || key == "farmers" || key == "soldiers" || key == "officers"
			|| key == "labourers" || key == "slaves")
		{
			V2Pop* pop = new V2Pop(*itr);
			pops.push_back(pop);
		}
	}
}

void V2Province::output() const
{
	FILE* output;
	if (fopen_s(&output, ("Output\\" + Configuration::getOutputName() + "\\history\\provinces\\" + filename).c_str(), "w") != 0)
	{
		int errNum;
		_get_errno(&errNum);
		char errStr[256];
		strerror_s(errStr, sizeof(errStr), errNum);
		LOG(LogLevel::Error) << "Could not create province history file Output\\" << Configuration::getOutputName() << "\\history\\provinces\\" << filename << " - " << errStr;
		exit(-1);
	}
	if (ownerString != "")
	{
		fprintf_s(output, "owner= %s\n", ownerString.c_str());
		fprintf_s(output, "controller= %s\n", ownerString.c_str());
	}
	for (unsigned int i = 0; i < cores.size(); i++)
	{
		fprintf_s(output, "add_core= %s\n", cores[i].c_str());
	}
	if(rgoType != "")
	{
		fprintf_s(output, "trade_goods = %s\n", rgoType.c_str());
	}
	if (lifeRating > 0)
	{
		fprintf_s(output, "life_rating = %d\n", lifeRating);
	}
	if (terrain != "")
	{
		fprintf_s(output, "terrain = %s\n", terrain.c_str());
	}
	if (colonial > 0)
	{
		if (Configuration::getV2Gametype() == "HOD")
		{
			fprintf(output, "colonial=%d\n", colonial);
		}
		else
		{
			fprintf(output, "colonial=yes\n");
		}
	}
	/*if (colonyLevel > 0)
	{
		fprintf_s(output, "colony = %d\n", colonyLevel);
	}*/
	if (navalBaseLevel > 0)
	{
		fprintf_s(output, "naval_base = %d\n", navalBaseLevel);
	}
	if (fortLevel > 0)
	{
		fprintf_s(output, "fort = %d\n", fortLevel);
	}
	if (railLevel > 0)
	{
		fprintf_s(output, "railroad = %d\n", railLevel);
	}
	if (slaveState)
	{
		fprintf_s(output, "is_slave = yes\n");
	}
	for (vector<const V2Factory*>::const_iterator itr = factories.begin(); itr != factories.end(); itr++)
	{
		(*itr)->output(output);
	}
	/*else if ((*itr)->getKey() == "party_loyalty")
	{
	}*/
	fclose(output);
}


void V2Province::outputPops(FILE* output) const
{
	if (ownerString == "")
	{
		for (unsigned int i = 0; i < oldPops.size(); i++)
		{
			oldPops[i]->output(output);
		}
	}
	else
	{
		for (unsigned int i = 0; i < pops.size(); i++)
		{
			pops[i]->output(output);
		}
	}
}


// determined experimentally
static const int unitNameOffsets[num_reg_categories] =
{
	16,	// infantry
	5,	// cavalry
	4,	// artillery
	19, // heavy_ship
	13, // light_ship
	0,	// galley (unused)
	6	// transport
};


void V2Province::outputUnits(FILE* output) const
{
	// unit name counts are stored in an odd kind of variable-length sparse array.  try to emulate.
	int outputUnitNameUntil = 0;
	for (int i = 0; i < num_reg_categories; ++i)
	{
		if (unitNameCount[i] > 0 && unitNameOffsets[i] > outputUnitNameUntil)
		{
			outputUnitNameUntil = unitNameOffsets[i];
		}
	}
	if (outputUnitNameUntil > 0)
	{
		fprintf(output, "\tunit_names=\n");
		fprintf(output, "\t{\n");
		fprintf(output, "\t\tdata=\n");
		fprintf(output, "\t\t{\n");
		for (int i = 1; i <= outputUnitNameUntil; ++i)
		{
			fprintf(output, "\t\t\t{\n");
			for (int j = 0; j < num_reg_categories; ++j)
			{
				if ((i == unitNameOffsets[j]) && unitNameCount[j] > 0)
				{
					fprintf(output, "\t\t\t\tcount=%d\n", unitNameCount[j]);
				}
			}
			fprintf(output, "\t\t\t}\n\n");
		}
		fprintf(output, "\t\t}\n");
		fprintf(output, "\t}\n");
	}
}


void V2Province::convertFromOldProvince(const EU4Province* oldProvince)
{
	srcProvince = oldProvince;
	if (oldProvince->isColony())
	{
		colonyLevel = 2;
	}
	colonial				= 0;
	colonised			= oldProvince->wasColonised();
	originallyInfidel	= oldProvince->wasInfidelConquest();
}


void V2Province::determineColonial()
{
	if ((!landConnection) && (!sameContinent) && ((colonised) || (originallyInfidel)))
	{
		colonial = 2;
	}
}

vector<V2Country*> V2Province::getCores(const map<string, V2Country*>& countries) const
{
	vector<V2Country*> coreOwners;
	for (vector<string>::const_iterator i = cores.begin(); i != cores.end(); i++)
	{
		map<string, V2Country*>::const_iterator j = countries.find(*i);
		if (j != countries.end())
		{
			coreOwners.push_back(j->second);
		}
	}

	return coreOwners;
}

void V2Province::addCore(string newCore)
{
	// only add if unique
	if ( find(cores.begin(), cores.end(), newCore) == cores.end() )
	{
		cores.push_back(newCore);
	}
}

void V2Province::removeCore(string tag)
{
	for (vector<string>::iterator i = cores.begin(); i != cores.end(); i++)
	{
		if (*i == tag)
		{
			cores.erase(i);
			if (cores.size() == 0)
			{
				break;
			}
			i = cores.begin();
		}
	}
}

void V2Province::addOldPop(const V2Pop* oldPop)
{
	oldPops.push_back(oldPop);
	oldPopulation += oldPop->getSize();
}


void V2Province::doCreatePops(bool isStateCapital, int statePopulation)
{
	for (vector<V2Demographic>::const_iterator itr = demographics.begin(); itr != demographics.end(); ++itr)
	{
		createPops(*itr, isStateCapital, statePopulation);
	}
	combinePops();
}


void V2Province::createPops(const V2Demographic& demographic, bool isStateCapital, int statePopulation)
{
	const EU4Province* oldProvince	= demographic.oldProvince;
	const EU4Country* oldCountry		= demographic.oldCountry;

	// each "point" here represents slightly less than 0.01% (1 / 10 000) population of this culture-religion pair
	// (depending on the total points allocated)
	int farmers			= 0;
	int labourers		= 0;
	int craftsmen		= 0;
	int slaves			= 0;
	int soldiers		= 0;
	int artisans		= 0;
	int clergymen		= 0;
	int clerks			= 0;
	int bureaucrats	= 0;
	int officers		= 0;
	int capitalists	= 0;
	int aristocrats	= 0;
	
	int govBuilding = 0;
	if (oldProvince->hasBuilding("temple"))
	{
		govBuilding = 1;
	}
	else if (oldProvince->hasBuilding("courthouse"))
	{
		govBuilding = 2;
	}
	else if (oldProvince->hasBuilding("spy_agency"))
	{
		govBuilding = 3;
	}
	else if (oldProvince->hasBuilding("town_hall"))
	{
		govBuilding = 4;
	}
	else if (oldProvince->hasBuilding("college"))
	{
		govBuilding = 6;
	}
	else if (oldProvince->hasBuilding("cathedral"))
	{
		govBuilding = 8;
	}

	int armyBuilding = 0;
	if (oldProvince->hasBuilding("armory"))
	{
		armyBuilding = 1;
	}
	else if (oldProvince->hasBuilding("training_fields"))
	{
		armyBuilding = 2;
	}
	else if (oldProvince->hasBuilding("barracks"))
	{
		armyBuilding = 3;
	}
	else if (oldProvince->hasBuilding("regimental_camp"))
	{
		armyBuilding = 4;
	}
	else if (oldProvince->hasBuilding("arsenal"))
	{
		armyBuilding = 6;
	}
	else if (oldProvince->hasBuilding("conscription_center"))
	{
		armyBuilding = 8;
	}

	int productionBuilding = 0;
	if (oldProvince->hasBuilding("constable"))
	{
		productionBuilding = 1;
	}
	else if (oldProvince->hasBuilding("workshop"))
	{
		productionBuilding = 2;
	}
	else if (oldProvince->hasBuilding("counting_house"))
	{
		productionBuilding = 3;
	}
	else if (oldProvince->hasBuilding("treasury_office"))
	{
		productionBuilding = 4;
	}
	else if (oldProvince->hasBuilding("mint"))
	{
		productionBuilding = 6;
	}
	else if (oldProvince->hasBuilding("stock_exchange"))
	{
		productionBuilding = 8;
	}


	if ( (rgoType == "cattle") || (rgoType == "coffee") || (rgoType == "cotton") || (rgoType == "dye") || (rgoType == "fish") || (rgoType == "fruit") || (rgoType == "grain") || (rgoType == "opium") || (rgoType == "silk") || (rgoType == "tea") || (rgoType == "tobacco") || (rgoType == "wool") )
	{
		farmers += 9000;
	}
	else
	{
		labourers += 9000;
	}

	if(   oldProvince->hasBuilding("weapons")
		|| oldProvince->hasBuilding("wharf")
		|| oldProvince->hasBuilding("refinery")
		|| oldProvince->hasBuilding("textile"))
	{
		craftsmen	+= (productionBuilding + 1) * 20;
		clerks		+= 5;
	}
	if (oldCountry->hasNationalIdea("scientific_revolution"))
	{
		clerks *= 3;
	}


	artisans	+= productionBuilding * 175;

	if (!oldCountry->hasModifier("the_abolish_slavery_act"))
	{
		slaves += 500;
	}

	soldiers += (armyBuilding + 1) * 8;
	if (oldCountry->hasNationalIdea("grand_army")
		|| oldCountry->hasNationalIdea("glorious_arms"))
	{
		soldiers *= 5;
	}

	officers	+= armyBuilding * 2;
	if (oldCountry->hasNationalIdea("battlefield_commissions")
		|| oldCountry->hasNationalIdea("sea_hawks"))
	{
		officers  *= 2;
	}

	clergymen += 100;
	if (oldCountry->hasNationalIdea("church_attendance_duty")
		|| oldCountry->hasNationalIdea("deus_vult"))
	{
		clergymen += 50;
	}

	bureaucrats += govBuilding * 2;
	if (oldCountry->getCapital() == oldProvince->getNum()
		|| oldCountry->getNationalFocus() == oldProvince->getNum())
	{
		bureaucrats	+= 30;
		aristocrats += 150;
	}
	if (oldCountry->hasNationalIdea("bureaucracy"))
	{
		bureaucrats *= 2;
	}

	aristocrats	+= 37;
	if ( (oldCountry->getGovernment() != "merchant_republic") && (oldCountry->getGovernment() != "noble_republic") && (oldCountry->getGovernment() != "administrative_republic") && (oldCountry->getGovernment() != "republican_dictatorship") && (oldCountry->getGovernment() != "constitutional_republic") && (oldCountry->getGovernment() != "bureaucratic_despotism") && (oldCountry->getGovernment() != "tribal_democracy") && (oldCountry->getGovernment() != "revolutionary_republic") )
	{
		aristocrats += 75;
	}
	if (oldCountry->hasNationalIdea("viceroys")
		&& oldProvince->wasColonised())
	{
		aristocrats += 38;
	}
	
	if (oldProvince->hasBuilding("customs_house"))
	{
		capitalists += 1;
	}
	if (oldCountry->getGovernment() != "absolute_monarchy")
	{
		capitalists *= 2;
	}
	if (oldCountry->hasNationalIdea("smithian_economics"))
	{
		capitalists *= 2;
	}

	// Uncivs cannot have capitalists, clerks, or craftsmen
	if (	(oldCountry->getTechGroup() != "western") &&
			(oldCountry->getTechGroup() != "latin") &&
			(oldCountry->getTechGroup() != "eastern") &&
			(oldCountry->getTechGroup() != "ottoman")
		)
	{
		capitalists	= 0;
		clerks		= 0;
		craftsmen	= 0;
	}

	//Bill of Rights NI reduces slaves by 10%
	if (oldCountry->hasNationalIdea("bill_of_rights") )
	{
		slaves = (int)(slaves * 0.9);
	}

	// Capitalists, Bureaucrats, and Aristocrats are only found in the state capital
	if (!isStateCapital)
	{
		capitalists = 0;
		bureaucrats = 0;
		aristocrats = 0;
	}
	else
	{
		double provPopRatio = 2.0f * (double)statePopulation / (double)oldPopulation;
		capitalists = (int)(capitalists * provPopRatio);
		bureaucrats = (int)(bureaucrats * provPopRatio);
		aristocrats = (int)(aristocrats * provPopRatio);
	}

	int total = farmers + labourers + slaves + soldiers + craftsmen + artisans + clergymen + clerks + bureaucrats + officers + capitalists + aristocrats;

	if (farmers > 0)
	{
		V2Pop* farmersPop = new V2Pop(	"farmers",
													(int)(demographic.ratio * oldPopulation * farmers / total),
													demographic.culture,
													demographic.religion
												);
		pops.push_back(farmersPop);
	}
	if (labourers > 0)
	{
		V2Pop* labourersPop = new V2Pop(	"labourers",
													(int)(demographic.ratio * oldPopulation * labourers / total),
													demographic.culture,
													demographic.religion
												);
		pops.push_back(labourersPop);
	}
	if (slaves > 0)
	{
		V2Pop* slavesPop = new V2Pop(		"slaves",
													(int)(demographic.ratio * oldPopulation * slaves / total),
													demographic.culture,
													demographic.religion
												);
		pops.push_back(slavesPop);
	}
	if (soldiers > 0)
	{
		V2Pop* soldiersPop = new V2Pop(	"soldiers",
													(int)(demographic.ratio * oldPopulation * soldiers / total),
													demographic.culture,
													demographic.religion
												);
		pops.push_back(soldiersPop);
	}
	if (craftsmen > 0)
	{
		V2Pop* craftsmenPop = new V2Pop(	"craftsmen",
													(int)(demographic.ratio * oldPopulation * craftsmen / total),
													demographic.culture,
													demographic.religion
												);
		pops.push_back(craftsmenPop);
	}
	if (artisans > 0)
	{
		V2Pop* artisansPop = new V2Pop(	"artisans",
													(int)(demographic.ratio * oldPopulation * artisans / total),
													demographic.culture,
													demographic.religion
												);
		pops.push_back(artisansPop);
	}
	if (clergymen > 0)
	{
		V2Pop* clergymenPop = new V2Pop(	"clergymen",
													(int)(demographic.ratio * oldPopulation * clergymen / total),
													demographic.culture,
													demographic.religion
												);
		pops.push_back(clergymenPop);
	}
	if (clerks > 0)
	{
		V2Pop* clerksPop = new V2Pop(	"clerks",
												(int)(demographic.ratio * oldPopulation * clerks / total),
												demographic.culture,
												demographic.religion
												);
		pops.push_back(clerksPop);
	}
	if (bureaucrats > 0)
	{
		V2Pop* bureaucratsPop = new V2Pop(	"bureaucrats",
														(int)(demographic.ratio * oldPopulation * bureaucrats / total),
														demographic.culture,
														demographic.religion
												);
		pops.push_back(bureaucratsPop);
	}
	if (officers > 0)
	{
		V2Pop* officersPop = new V2Pop(	"officers",
													(int)(demographic.ratio * oldPopulation * officers / total),
													demographic.culture,
													demographic.religion
												);
		pops.push_back(officersPop);
	}
	if (capitalists > 0)
	{
		V2Pop* capitalistsPop = new V2Pop(	"capitalists",
														(int)(demographic.ratio * oldPopulation * capitalists / total),
														demographic.culture,
														demographic.religion
												);
		pops.push_back(capitalistsPop);
	}
	if (aristocrats > 0)
	{
		V2Pop* aristocratsPop = new V2Pop(	"aristocrats",
														(int)(demographic.ratio * oldPopulation * aristocrats / total),
														demographic.culture,
														demographic.religion
												);
		pops.push_back(aristocratsPop);
	}
}


void V2Province::combinePops()
{
	vector<V2Pop*> trashPops;
	for (vector<V2Pop*>::iterator lhs = pops.begin(); lhs != pops.end(); ++lhs)
	{
		vector<V2Pop*>::iterator rhs = lhs;
		for (++rhs; rhs != pops.end(); ++rhs)
		{
			if ( (*lhs)->combine(**rhs) )
			{
				trashPops.push_back(*rhs);
			}
			if ( (*rhs)->getSize() < 1 )
			{
				trashPops.push_back(*rhs);
			}
		}
	}

	vector<V2Pop*> consolidatedPops;
	for (vector<V2Pop*>::iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		bool isTrashed = false;
		for (vector<V2Pop*>::iterator titr = trashPops.begin(); titr != trashPops.end(); ++titr)
		{
			if (*itr == *titr)
			{
				isTrashed = true;
			}
		}
		if (!isTrashed)
		{
			consolidatedPops.push_back(*itr);
		}
	}
	pops.swap(consolidatedPops);
}


bool PopSortForOutputPredicate(const V2Pop* pop1, const V2Pop* pop2)
{
	if (pop1->getType() != pop2->getType())
	{
		return pop1->getType() < pop2->getType();
	}
	return pop1->getID() < pop2->getID();
}


void V2Province::sortPops()
{
	sort(oldPops.begin(), oldPops.end(), PopSortForOutputPredicate);
	sort(pops.begin(), pops.end(), PopSortForOutputPredicate);
}


int V2Province::getTotalPopulation() const
{
	int total = 0;
	for (vector<V2Pop*>::const_iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		total += (*itr)->getSize();
	}
	return total;
}


vector<V2Pop*> V2Province::getPops(string type) const
{
	vector<V2Pop*> retval;
	for (vector<V2Pop*>::const_iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		if (type == "*" || (*itr)->getType() == type)
			retval.push_back(*itr);
	}
	return retval;
}


static bool PopSortBySizePredicate(const V2Pop* pop1, const V2Pop* pop2)
{
	return (pop1->getSize() > pop2->getSize());
}


// V2 requires 1000 for the first regiment and 3000 thereafter
// we require an extra 1/30 to stabilize the start of the game
static int getRequiredPopForRegimentCount(int count)
{
	if (count == 0) return 0;
	return (1033 + (count - 1) * 3100);
}


// pick a soldier pop to use for an army.  prefer larger pops to smaller ones, and grow only if necessary.
int V2Province::getSoldierPopForArmy(bool force)
{
	vector<V2Pop*> spops = getPops("soldiers");
	if (spops.size() == 0)
		return -1; // no soldier pops

	sort(spops.begin(), spops.end(), PopSortBySizePredicate);
	// try largest to smallest, without growing
	for (vector<V2Pop*>::iterator itr = spops.begin(); itr != spops.end(); ++itr)
	{
		int growBy = getRequiredPopForRegimentCount( (*itr)->getSupportedRegimentCount() + 1 ) - (*itr)->getSize();
		if (growBy <= 0)
		{
			if (growSoldierPop(*itr)) // won't actually grow, but necessary to increment supported regiment count
				return (*itr)->getID();
		}
	}
	// try largest to smallest, trying to grow
	for (vector<V2Pop*>::iterator itr = spops.begin(); itr != spops.end(); ++itr)
	{
		if (growSoldierPop(*itr))
			return (*itr)->getID();
	}

	// no suitable pops
	if (force)
		return spops[0]->getID();
	else
		return -1;
}


bool V2Province::growSoldierPop(V2Pop* pop)
{
	int growBy = getRequiredPopForRegimentCount(pop->getSupportedRegimentCount() + 1) - pop->getSize();
	if (growBy > 0)
	{
		// gotta grow; find a same-culture same-religion farmer/laborer to pull from
		int provincePop = getTotalPopulation();
		bool foundSourcePop = false;
		for (vector<V2Pop*>::iterator isrc = pops.begin(); isrc != pops.end(); ++isrc)
		{
			if ( (*isrc)->getType() == "farmers" || (*isrc)->getType() == "labourers" )
			{
				if ( (*isrc)->getCulture() == pop->getCulture() && (*isrc)->getReligion() == pop->getReligion() )
				{
					// don't let the farmer/labourer shrink beneath 10% of the province population
					if ( (*isrc)->getSize() - growBy > provincePop * 0.10 )
					{
						(*isrc)->changeSize(-growBy);
						pop->changeSize(growBy);
						foundSourcePop = true;
						break;
					}
				}
			}
		}
		if (!foundSourcePop)
		{
			return false;
		}
	}
	pop->incrementSupportedRegimentCount();
	return true;
}


pair<int, int> V2Province::getAvailableSoldierCapacity() const
{
	int soldierCap = 0;
	int draftCap = 0;
	int provincePop = getTotalPopulation();
	for (vector<V2Pop*>::const_iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		if ( (*itr)->getType() == "soldiers" )
		{
			// unused capacity is the size of the pop minus the capacity already used, or 0, if it's already overdrawn
			soldierCap += max( (*itr)->getSize() - getRequiredPopForRegimentCount( (*itr)->getSupportedRegimentCount() ), 0 );
		}
		else if ( (*itr)->getType() == "farmers" || (*itr)->getType() == "labourers" )
		{
			// unused capacity is the size of the pop in excess of 10% of the province pop, or 0, if it's already too small
			draftCap += max( (*itr)->getSize() - int(0.10 * provincePop), 0 );
		}
	}
	return pair<int,int>(soldierCap, draftCap);
}


static string CardinalToOrdinal(int cardinal)
{
	int hundredRem = cardinal % 100;
	int tenRem = cardinal % 10;
	if (hundredRem - tenRem == 10)
	{
		return "th";
	}

	switch (tenRem)
	{
	case 1:
		return "st";
	case 2:
		return "nd";
	case 3:
		return "rd";
	default:
		return "th";
	}
}


string V2Province::getRegimentName(RegimentCategory rc)
{
	// galleys turn into light ships; count and name them identically
	if (rc == galley)
		rc = light_ship;

	stringstream str;
	str << ++unitNameCount[rc] << CardinalToOrdinal(unitNameCount[rc]); // 1st, 2nd, etc
	str << " " << name << " "; // Hamburg, Lyon, etc
	switch (rc)
	{
		case artillery:
			str << "Artillery";
			break;
		case infantry:
			str << "Infantry";
			break;
		case cavalry:
			str << "Cavalry";
			break;
		case heavy_ship:
			str << "Man'o'war";
			break;
		case light_ship:
			str << "Frigate";
			break;
		case transport:
			str << "Clipper Transport";
			break;
	}
	return str.str();
}


bool V2Province::hasCulture(string culture, float percentOfPopulation) const
{
	int culturePops = 0;
	for (vector<V2Pop*>::const_iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		if ( (*itr)->getCulture() == culture )
		{
			culturePops += (*itr)->getSize();
		}
	}

	return ((float)culturePops / getTotalPopulation()) >= percentOfPopulation;
}
int V2Province::getPopulation(string type) const
{
	int totalPop = 0;
	for (vector<V2Pop*>::const_iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		// empty string for type gets total population
		if (type == "" || type == (*itr)->getType())
		{
			totalPop += (*itr)->getSize();
		}
	}
	return totalPop;
}

int V2Province::getLiteracyWeightedPopulation(string type) const
{
	double literacyWeight = Configuration::getLiteracyWeight();
	int totalPop = 0;
	for (vector<V2Pop*>::const_iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		// empty string for type gets total population
		if (type == "" || type == (*itr)->getType())
		{
			totalPop += int((*itr)->getSize() * ((*itr)->getLiteracy() * literacyWeight + (1.0 - literacyWeight)));
		}
	}
	return totalPop;
}
