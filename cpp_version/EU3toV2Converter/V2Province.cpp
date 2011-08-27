#include "V2Province.h"
#include "Log.h"

#include <sstream>
#include <algorithm>


void V2Province::init(int newNumber)
{
	num				= newNumber;
	name				= "";
	land				= false;
	oldPopulation	= 0;
	coastal				= false;

	for (int i = 0; i < num_reg_categories; ++i)
		unitNameCount[i] = 0;
}


int V2Province::getNum()
{
	return num;
}


string V2Province::getName()
{
	return name;
}


void V2Province::setName(string newName)
{
	name = newName;
}


void V2Province::setOwner(string newOwner)
{
	owner = newOwner;
}


void V2Province::addCore(string newCore)
{
	// only add if unique
	if (std::find(cores.begin(), cores.end(), newCore) == cores.end())
		cores.push_back(newCore);
}


void V2Province::setColonial(bool isIt)
{
	colonial = isIt;
}


string V2Province::getCulture()
{
	map<string, int> culturePop;
	pair<string, int> currentPlurality;
	for (vector<V2Pop>::iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		map<string, int>::iterator thisCulture = culturePop.find(itr->getCulture());
		if (thisCulture == culturePop.end())
			thisCulture = culturePop.insert(thisCulture, pair<string, int>(itr->getCulture(), 0));
		thisCulture->second += itr->getSize();
		if (thisCulture->second > currentPlurality.second)
			currentPlurality = (*thisCulture);
	}
	return currentPlurality.first;
}


void V2Province::setRgoType(string newRgo)
{
	rgoType = newRgo;
	land = true;
}


void V2Province::setLifeRating(int newLifeRating)
{
	lifeRating = newLifeRating;
}


void V2Province::setColonised(bool wasIt)
{
	colonised = wasIt;
}


bool V2Province::wasColonised()
{
	return colonised;
}


bool V2Province::isColonial()
{
	return colonial;
}

void V2Province::addOldPop(V2Pop oldPop)
{
	oldPops.push_back(oldPop);
	oldPopulation += oldPop.getSize();
}


string V2Province::getOwner()
{
	return owner;
}


vector<V2Pop> V2Province::getPops(string type)
{
	vector<V2Pop> retval;
	for (vector<V2Pop>::iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		if (type == "*" || itr->getType() == type)
			retval.push_back(*itr);
	}
	return retval;
}


bool V2Province::isCoastal()
{
	return coastal;
}


void V2Province::setCoastal(bool _coastal)
{
	coastal = _coastal;
}


void V2Province::createPops(string culture, string religion, double ratio, EU3Province* oldProvince, EU3Country* oldCountry)
{
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

	if ( (rgoType == "cattle") || (rgoType == "coffee") || (rgoType == "cotton") || (rgoType == "dye") || (rgoType == "fish") || (rgoType == "fruit") || (rgoType == "grain") || (rgoType == "opium") || (rgoType == "silk") || (rgoType == "tea") || (rgoType == "tobacco") || (rgoType == "wool") )
	{
		farmers += 90;
	}
	else
	{
		labourers += 90;
		craftsmen += 5;
	}

	if (true)  //If Nation has Slavery ALLOWED, add 5 SLAVES. Assumed true for now
	{
		slaves += 5;
	}
	soldiers	+= 2;
	if(0)	//If province has a MANUFACTORY
	{
		craftsmen	+= 2;
		clerks		+= 1;
	}
	artisans		+= 5;
	clergymen	+= 1;
	officers		+= 1;
	//If province is the CAPITAL or NATIONAL FOCUS add 1 BUREAUCRAT
	//If province is capital or was national focus, add 1 ARISTOCRATS.
	//If province is CENTER OF TRADE then add 1 CLERK
	//If province was a COT, add 1 CAPITALISTS.
	//If government is NOT republic, add 2 ARISTOCRATS.
	//If government is NOT absolute monarchy, add 2 CAPITALISTS.
	aristocrats	+= 1; // temporary for now

	int total = farmers + labourers + slaves + soldiers + craftsmen + artisans + clergymen + clerks + bureaucrats + officers + capitalists + aristocrats;

	if (farmers > 0)
	{
		V2Pop farmersPop;
		farmersPop.setType("farmers");
		farmersPop.setSize((int)(ratio * oldPopulation * farmers / total));
		farmersPop.setCulture(culture);
		farmersPop.setReligion(religion);
		pops.push_back(farmersPop);
	}
	if (labourers > 0)
	{
		V2Pop labourersPop;
		labourersPop.setType("labourers");
		labourersPop.setSize((int)(ratio * oldPopulation * labourers / total));
		labourersPop.setCulture(culture);
		labourersPop.setReligion(religion);
		pops.push_back(labourersPop);
	}
	if (slaves > 0)
	{
		V2Pop slavesPop;
		slavesPop.setType("slaves");
		slavesPop.setSize((int)(ratio * oldPopulation * slaves / total));
		slavesPop.setCulture(culture);
		slavesPop.setReligion(religion);
		pops.push_back(slavesPop);
	}
	if (soldiers > 0)
	{
		V2Pop soldiersPop;
		soldiersPop.setType("soldiers");
		soldiersPop.setSize((int)(ratio * oldPopulation * soldiers / total));
		soldiersPop.setCulture(culture);
		soldiersPop.setReligion(religion);
		pops.push_back(soldiersPop);
	}
	if (craftsmen > 0)
	{
		V2Pop craftsmenPop;
		craftsmenPop.setType("craftsmen");
		craftsmenPop.setSize((int)(ratio * oldPopulation * craftsmen / total));
		craftsmenPop.setCulture(culture);
		craftsmenPop.setReligion(religion);
		pops.push_back(craftsmenPop);
	}
	if (artisans > 0)
	{
		V2Pop artisansPop;
		artisansPop.setType("artisans");
		artisansPop.setSize((int)(ratio * oldPopulation * artisans / total));
		artisansPop.setCulture(culture);
		artisansPop.setReligion(religion);
		pops.push_back(artisansPop);
	}
	if (clergymen > 0)
	{
		V2Pop clergymenPop;
		clergymenPop.setType("clergymen");
		clergymenPop.setSize((int)(ratio * oldPopulation * clergymen / total));
		clergymenPop.setCulture(culture);
		clergymenPop.setReligion(religion);
		pops.push_back(clergymenPop);
	}
	if (clerks > 0)
	{
		V2Pop clerksPop;
		clerksPop.setType("clerks");
		clerksPop.setSize((int)(ratio * oldPopulation * clerks / total));
		clerksPop.setCulture(culture);
		clerksPop.setReligion(religion);
		pops.push_back(clerksPop);
	}
	if (bureaucrats > 0)
	{
		V2Pop bureaucratsPop;
		bureaucratsPop.setType("bureaucrats");
		bureaucratsPop.setSize((int)(ratio * oldPopulation * bureaucrats / total));
		bureaucratsPop.setCulture(culture);
		bureaucratsPop.setReligion(religion);
		pops.push_back(bureaucratsPop);
	}
	if (officers > 0)
	{
		V2Pop officersPop;
		officersPop.setType("officers");
		officersPop.setSize((int)(ratio * oldPopulation * officers / total));
		officersPop.setCulture(culture);
		officersPop.setReligion(religion);
		pops.push_back(officersPop);
	}
	if (capitalists > 0)
	{
		V2Pop capitalistsPop;
		capitalistsPop.setType("capitalists");
		capitalistsPop.setSize((int)(ratio * oldPopulation * capitalists / total));
		capitalistsPop.setCulture(culture);
		capitalistsPop.setReligion(religion);
		pops.push_back(capitalistsPop);
	}
	if (aristocrats > 0)
	{
		V2Pop aristocratsPop;
		aristocratsPop.setType("aristocrats");
		aristocratsPop.setSize((int)(ratio * oldPopulation * aristocrats / total));
		aristocratsPop.setCulture(culture);
		aristocratsPop.setReligion(religion);
		pops.push_back(aristocratsPop);
	}
}


int V2Province::getTotalPopulation() const
{
	int total = 0;
	for (vector<V2Pop>::const_iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		total += itr->getSize();
	}
	return total;
}


// V2 requires 1000 for the first regiment and 3000 thereafter
// we require an extra 1/30 to stabilize the start of the game
static int getRequiredPopForRegimentCount(int count)
{
	if (count == 0) return 0;
	return (1033 + (count - 1) * 3100);
}

bool V2Province::growSoldierPop(int popID)
{
	for (vector<V2Pop>::iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		if (itr->getID() == popID)
		{
			int currentlySupported = itr->getSupportedRegimentCount();
			int growBy = getRequiredPopForRegimentCount(currentlySupported + 1) - itr->getSize();
			if (growBy > 0)
			{
				// gotta grow; find a same-culture same-religion farmer/laborer to pull from
				int provincePop = getTotalPopulation();
				bool foundSourcePop = false;
				for (vector<V2Pop>::iterator isrc = pops.begin(); isrc != pops.end(); ++isrc)
				{
					if (isrc->getType() == "farmers" || isrc->getType() == "labourers")
					{
						if (isrc->getCulture() == itr->getCulture() && isrc->getReligion() == itr->getReligion())
						{
							// don't let the farmer/labourer shrink beneath 10% of the province population
							if (isrc->getSize() - growBy > provincePop * 0.10)
							{
								isrc->setSize(isrc->getSize() - growBy);
								itr->setSize(itr->getSize() + growBy);
								foundSourcePop = true;
								break;
							}
						}
					}
				}
				if (!foundSourcePop)
					return false;
			}
			itr->setSupportedRegimentCount(++currentlySupported);
			return true;
		}
	}
	return false;
}


static bool PopSortBySizePredicate(const V2Pop& pop1, const V2Pop& pop2)
{
	return (pop1.getSize() > pop2.getSize());
}

// pick a soldier pop to use for an army.  prefer larger pops to smaller ones, and grow only if necessary.
int V2Province::getSoldierPopForArmy(bool force)
{
	vector<V2Pop> spops = getPops("soldiers");
	if (spops.size() == 0)
		return -1; // no soldier pops

	sort(spops.begin(), spops.end(), PopSortBySizePredicate);
	// try largest to smallest, without growing
	for (vector<V2Pop>::iterator itr = spops.begin(); itr != spops.end(); ++itr)
	{
		int growBy = getRequiredPopForRegimentCount(itr->getSupportedRegimentCount() + 1) - itr->getSize();
		if (growBy <= 0)
		{
			if (growSoldierPop(itr->getID())) // won't actually grow, but necessary to increment supported regiment count
				return itr->getID();
		}
	}
	// try largest to smallest, trying to grow
	for (vector<V2Pop>::iterator itr = spops.begin(); itr != spops.end(); ++itr)
	{
		if (growSoldierPop(itr->getID()))
			return itr->getID();
	}

	// no suitable pops
	if (force)
		return spops[0].getID();
	else
		return -1;
}


pair<int, int> V2Province::getAvailableSoldierCapacity() const
{
	int soldierCap = 0;
	int draftCap = 0;
	int provincePop = getTotalPopulation();
	for (vector<V2Pop>::const_iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		if (itr->getType() == "soldiers")
		{
			// unused capacity is the size of the pop minus the capacity already used, or 0, if it's already overdrawn
			soldierCap += max(itr->getSize() - getRequiredPopForRegimentCount(itr->getSupportedRegimentCount()), 0);
		}
		else if (itr->getType() == "farmers" || itr->getType() == "labourers")
		{
			// unused capacity is the size of the pop in excess of 10% of the province pop, or 0, if it's already too small
			draftCap += max(itr->getSize() - int(0.10 * provincePop), 0);
		}
	}
	return pair<int,int>(soldierCap, draftCap);
}


void V2Province::combinePops()
{
	vector<int> trashPops;
	for (vector<V2Pop>::iterator lhs = pops.begin(); lhs != pops.end(); ++lhs)
	{
		vector<V2Pop>::iterator rhs = lhs;
		for (++rhs; rhs != pops.end(); ++rhs)
		{
			if (lhs->canCombine(*rhs))
			{
				lhs->setSize(lhs->getSize() + rhs->getSize());
				trashPops.push_back(rhs->getID());
			}
			if (rhs->getSize() < 1)
				trashPops.push_back(rhs->getID());
		}
	}
	vector<V2Pop> consolidatedPops;
	for (vector<V2Pop>::iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		bool isTrashed = false;
		for (vector<int>::iterator titr = trashPops.begin(); titr != trashPops.end(); ++titr)
		{
			if (itr->getID() == (*titr))
				isTrashed = true;
		}
		if (!isTrashed)
			consolidatedPops.push_back(*itr);
	}
	pops.swap(consolidatedPops);
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
	str << " " << getName() << " "; // Hamburg, Lyon, etc
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
	case big_ship:
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


// determined experimentally
static const int unitNameOffsets[num_reg_categories] =
{
	16,	// infantry
	5,	// cavalry
	4,	// artillery
	19, // big_ship
	13, // light_ship
	0,	// galley (unused)
	6	// transport
};

bool PopSortForOutputPredicate(const V2Pop& pop1, const V2Pop& pop2)
{
	if (pop1.getType() != pop2.getType())
	{
		return pop1.getType() < pop2.getType();
	}
	return pop1.getID() < pop2.getID();
}

void V2Province::output(FILE* output)
{
	if (name == "")
	{
		log("Error: province %d was never assigned a name.\n", num);
	}
	fprintf(output, "%d=\n", num);
	fprintf(output, "{\n");
	fprintf(output, "	name=\"%s\"\n", name.c_str());
	if (land)
	{
		if (owner != "")
		{
			fprintf(output, "	owner=\"%s\"\n", owner.c_str());
			fprintf(output, "	controller=\"%s\"\n", owner.c_str());
		}
		for (unsigned int i = 0; i < cores.size(); i++)
		{
			fprintf(output, "	core=\"%s\"\n", cores[i].c_str());
		}
	}
	fprintf(output, "	garrison=100.000\n");
	if (land)
	{
		if (owner == "")
		{
			sort(oldPops.begin(), oldPops.end(), PopSortForOutputPredicate);
			for (unsigned int i = 0; i < oldPops.size(); i++)
			{
				oldPops[i].output(output);
			}
		}
		else
		{
			sort(pops.begin(), pops.end(), PopSortForOutputPredicate);
			for (unsigned int i = 0; i < pops.size(); i++)
			{
				pops[i].output(output);
			}
		}
		fprintf(output, "	rgo=\n");
		fprintf(output, "	{\n");
		fprintf(output, "		employment=\n");
		fprintf(output, "		{\n");
		fprintf(output, "			province_id=%d\n", num);
		fprintf(output, "			employees=\n");
		fprintf(output, "			{\n");
		if (owner == "")
		{
			int numFarmers = 0;
			for (unsigned int i = 0; i < oldPops.size(); i++)
			{
				if (oldPops[i].getType() == "farmers")
				{
					fprintf(output, "				{\n");
					fprintf(output, "					province_pop_id=\n");
					fprintf(output, "					{\n");
					fprintf(output, "						province_id=%d\n", num);
					fprintf(output, "						index=%d\n", numFarmers);
					fprintf(output, "						type=8\n");
					fprintf(output, "					}\n");
					fprintf(output, "					count=%d\n", oldPops[i].getSize());
					fprintf(output, "				}\n");
					numFarmers++;
				}
			}
			int numLabourers = 0;
			for (unsigned int i = 0; i < oldPops.size(); i++)
			{
				if (oldPops[i].getType() == "labourers")
				{
					fprintf(output, "				{\n");
					fprintf(output, "					province_pop_id=\n");
					fprintf(output, "					{\n");
					fprintf(output, "						province_id=%d\n", num);
					fprintf(output, "						index=%d\n", numLabourers);
					fprintf(output, "						type=9\n");
					fprintf(output, "					}\n");
					fprintf(output, "					count=%d\n", oldPops[i].getSize());
					fprintf(output, "				}\n");
					numLabourers++;
				}
			}
			int numSlaves = 0;
			for (unsigned int i = 0; i < oldPops.size(); i++)
			{
				if (oldPops[i].getType() == "slaves")
				{
					fprintf(output, "				{\n");
					fprintf(output, "					province_pop_id=\n");
					fprintf(output, "					{\n");
					fprintf(output, "						province_id=%d\n", num);
					fprintf(output, "						index=%d\n", numSlaves);
					fprintf(output, "						type=11\n");
					fprintf(output, "					}\n");
					fprintf(output, "					count=%d\n", oldPops[i].getSize());
					fprintf(output, "				}\n");
					numSlaves++;
				}
			}
		}
		else
		{
			int numFarmers = 0;
			for (unsigned int i = 0; i < pops.size(); i++)
			{
				if (pops[i].getType() == "farmers")
				{
					fprintf(output, "				{\n");
					fprintf(output, "					province_pop_id=\n");
					fprintf(output, "					{\n");
					fprintf(output, "						province_id=%d\n", num);
					fprintf(output, "						index=%d\n", numFarmers);
					fprintf(output, "						type=8\n");
					fprintf(output, "					}\n");
					fprintf(output, "					count=%d\n", pops[i].getSize());
					fprintf(output, "				}\n");
					numFarmers++;
				}
			}
			int numLabourers = 0;
			for (unsigned int i = 0; i < pops.size(); i++)
			{
				if (pops[i].getType() == "labourers")
				{
					fprintf(output, "				{\n");
					fprintf(output, "					province_pop_id=\n");
					fprintf(output, "					{\n");
					fprintf(output, "						province_id=%d\n", num);
					fprintf(output, "						index=%d\n", numLabourers);
					fprintf(output, "						type=9\n");
					fprintf(output, "					}\n");
					fprintf(output, "					count=%d\n", pops[i].getSize());
					fprintf(output, "				}\n");
					numLabourers++;
				}
			}
			int numSlaves = 0;
			for (unsigned int i = 0; i < pops.size(); i++)
			{
				if (pops[i].getType() == "slaves")
				{
					fprintf(output, "				{\n");
					fprintf(output, "					province_pop_id=\n");
					fprintf(output, "					{\n");
					fprintf(output, "						province_id=%d\n", num);
					fprintf(output, "						index=%d\n", numSlaves);
					fprintf(output, "						type=11\n");
					fprintf(output, "					}\n");
					fprintf(output, "					count=%d\n", pops[i].getSize());
					fprintf(output, "				}\n");
					numSlaves++;
				}
			}
		}
		fprintf(output, "			}\n");
		fprintf(output, "		}\n");
		fprintf(output, "		last_income=100000.00000\n");
		fprintf(output, "		goods_type=\"%s\"\n", rgoType.c_str());
		fprintf(output, "	}\n");
		fprintf(output, "	life_rating=%d\n", lifeRating);

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
	fprintf(output, "}\n");
}