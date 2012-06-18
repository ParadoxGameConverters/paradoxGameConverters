#include "V2Province.h"
#include "Log.h"
#include "EU3World.h"
#include "EU3Province.h"
#include "V2Pop.h"
#include "V2Country.h"
#include <sstream>
#include <algorithm>



void V2Province::init(int newNumber)
{
	num				= newNumber;
	name				= "";
	owner				= "";
	land				= false;
	oldPopulation	= 0;
	coastal				= false;
	fortLevel			= 0;
	navalBaseLevel		= 0;
	railLevel			= 0;

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
	owner		= newOwner;
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
	for (vector<V2Pop*>::iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		map<string, int>::iterator thisCulture = culturePop.find( (*itr)->getCulture() );
		if (thisCulture == culturePop.end())
			thisCulture = culturePop.insert(thisCulture, pair<string, int>( (*itr)->getCulture(), 0) );
		thisCulture->second += (*itr)->getSize();
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


string V2Province::getRgoType()
{
	return rgoType;
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


void V2Province::setPaganConquest(bool wasIt)
{
	originallyPagan = wasIt;
}


bool V2Province::wasPaganConquest()
{
	return originallyPagan;
}


void V2Province::setCOT(bool wasCOT)
{
	COT = wasCOT;
}


bool V2Province::getCOT()
{
	return COT;
}


bool V2Province::isColonial()
{
	return colonial;
}

void V2Province::addOldPop(V2Pop* oldPop)
{
	oldPops.push_back(oldPop);
	oldPopulation += oldPop->getSize();
}


string V2Province::getOwner()
{
	return owner;
}


vector<V2Pop*> V2Province::getPops(string type)
{
	vector<V2Pop*> retval;
	for (vector<V2Pop*>::iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		if (type == "*" || (*itr)->getType() == type)
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


bool V2Province::isLand()
{
	return land;
}


void V2Province::addPopDemographic(V2Demographic d)
{
	demographics.push_back(d);
}


void V2Province::doCreatePops(bool isStateCapital, int statePopulation, EU3World& sourceWorld, bool stateHasCOT)
{
	for (vector<V2Demographic>::iterator itr = demographics.begin(); itr != demographics.end(); ++itr)
	{
		EU3Province* oldProvince = sourceWorld.getProvince(itr->oldProvinceID);
		EU3Country* oldCountry = sourceWorld.getCountry(itr->oldCountry);
		createPops(*itr, isStateCapital, statePopulation, oldProvince, oldCountry, stateHasCOT);
	}
	combinePops();
}


void V2Province::createPops(const V2Demographic& demographic, bool isStateCapital, int statePopulation, EU3Province* oldProvince, EU3Country* oldCountry, bool stateHasCOT)
{
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

	if (Configuration::getEU3Gametype() == "httt")
	{
		if ( (rgoType == "cattle") || (rgoType == "coffee") || (rgoType == "cotton") || (rgoType == "dye") || (rgoType == "fish") || (rgoType == "fruit") || (rgoType == "grain") || (rgoType == "opium") || (rgoType == "silk") || (rgoType == "tea") || (rgoType == "tobacco") || (rgoType == "wool") )
		{
			farmers += 9000;
		}
		else
		{
			labourers += 9000;
			craftsmen += 100;
		}

		if(   oldProvince->hasBuilding("weapons")
		   || oldProvince->hasBuilding("wharf")
			|| oldProvince->hasBuilding("refinery")
		   || oldProvince->hasBuilding("textile"))
		{
			craftsmen	+= 100;
			clerks		+= 5;
		}
		if (oldCountry->hasNationalIdea("scientific_revolution"))
		{
			clerks += 10;
		}

		artisans	+= 800;

		//If province is CENTER OF TRADE then add 100 CLERKS, 3 CAPITALISTS, 1000 ARTISANS.
		//if ()
		//{
		//}

		if (!oldCountry->hasModifier("the_abolish_slavery_act"))
		{
			slaves += 500;
		}

		soldiers	+= 50;
		if (oldCountry->hasNationalIdea("grand_army")
			|| oldCountry->hasNationalIdea("glorious_arms"))
		{
			soldiers += 50;
		}

		officers		+= 100;
		if (oldCountry->hasNationalIdea("battlefield_commissions")
			|| oldCountry->hasNationalIdea("sea_hawks"))
		{
			officers  += 10;
		}

		clergymen	+= 100;
		if (oldCountry->hasNationalIdea("church_attendance_duty")
			|| oldCountry->hasNationalIdea("deus_vult"))
		{
			clergymen += 50;
		}

		bureaucrats += 7;
		if (oldCountry->getCapital() == oldProvince->getNum()
			|| oldCountry->getNationalFocus() == oldProvince->getNum())
		{
			bureaucrats	+= 30;
			aristocrats += 100;
		}
		if (oldCountry->hasNationalIdea("bureaucracy"))
		{
			bureaucrats += 7;
		}

		aristocrats	+= 25;
		if ( (oldCountry->getGovernment() != "merchant_republic") && (oldCountry->getGovernment() != "noble_republic") && (oldCountry->getGovernment() != "administrative_republic") && (oldCountry->getGovernment() != "republican_dictatorship") && (oldCountry->getGovernment() != "constitutional_republic") && (oldCountry->getGovernment() != "bureaucratic_despotism") && (oldCountry->getGovernment() != "tribal_democracy") && (oldCountry->getGovernment() != "revolutionary_republic") )
		{
			aristocrats += 50;
		}
		if (oldCountry->hasNationalIdea("viceroys")
			&& oldProvince->wasColonised())
		{
			aristocrats += 25;
		}
	
		if (oldCountry->getGovernment() != "absolute_monarchy")
		{
			capitalists += 2;
		}
		if (oldCountry->hasNationalIdea("smithian_economics"))
		{
			capitalists += 1;
		}
	}

	else // Gametype == dw
	{
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

		//If province is CENTER OF TRADE then add 10 CLERKS, 3 CAPITALISTS, 100 ARTISANS.
		if (stateHasCOT)
		{
			artisans		+= 875;
			clerks		+= 10;
			capitalists	+= 3;
		}

		if (!oldCountry->hasModifier("the_abolish_slavery_act"))
		{
			slaves += 500;
		}

		soldiers	+= armyBuilding * 10;
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
		V2Pop* farmersPop = new V2Pop;
		farmersPop->setType("farmers");
		farmersPop->setSize((int)(demographic.ratio * oldPopulation * farmers / total));
		farmersPop->setCulture(demographic.culture);
		farmersPop->setReligion(demographic.religion);
		farmersPop->setLiteracy(demographic.literacy);
		farmersPop->setIdeology(demographic.reactionary, demographic.conservative, demographic.liberal);
		farmersPop->setIssues(demographic.issues);
		pops.push_back(farmersPop);
	}
	if (labourers > 0)
	{
		V2Pop* labourersPop = new V2Pop;
		labourersPop->setType("labourers");
		labourersPop->setSize((int)(demographic.ratio * oldPopulation * labourers / total));
		labourersPop->setCulture(demographic.culture);
		labourersPop->setReligion(demographic.religion);
		labourersPop->setLiteracy(demographic.literacy);
		labourersPop->setIdeology(demographic.reactionary, demographic.conservative, demographic.liberal);
		labourersPop->setIssues(demographic.issues);
		pops.push_back(labourersPop);
	}
	if (slaves > 0)
	{
		V2Pop* slavesPop = new V2Pop;
		slavesPop->setType("slaves");
		slavesPop->setSize((int)(demographic.ratio * oldPopulation * slaves / total));
		slavesPop->setCulture(demographic.culture);
		slavesPop->setReligion(demographic.religion);
		slavesPop->setLiteracy(demographic.literacy);
		slavesPop->setIdeology(demographic.reactionary, demographic.conservative, demographic.liberal);
		slavesPop->setIssues(demographic.issues);
		pops.push_back(slavesPop);
	}
	if (soldiers > 0)
	{
		V2Pop* soldiersPop = new V2Pop;
		soldiersPop->setType("soldiers");
		soldiersPop->setSize((int)(demographic.ratio * oldPopulation * soldiers / total));
		soldiersPop->setCulture(demographic.culture);
		soldiersPop->setReligion(demographic.religion);
		soldiersPop->setLiteracy(demographic.literacy);
		soldiersPop->setIdeology(demographic.reactionary, demographic.conservative, demographic.liberal);
		soldiersPop->setIssues(demographic.issues);
		pops.push_back(soldiersPop);
	}
	if (craftsmen > 0)
	{
		V2Pop* craftsmenPop = new V2Pop;
		craftsmenPop->setType("craftsmen");
		craftsmenPop->setSize((int)(demographic.ratio * oldPopulation * craftsmen / total));
		craftsmenPop->setCulture(demographic.culture);
		craftsmenPop->setReligion(demographic.religion);
		craftsmenPop->setLiteracy(demographic.literacy);
		craftsmenPop->setIdeology(demographic.reactionary, demographic.conservative, demographic.liberal);
		craftsmenPop->setIssues(demographic.issues);
		pops.push_back(craftsmenPop);
	}
	if (artisans > 0)
	{
		V2Pop* artisansPop = new V2Pop;
		artisansPop->setType("artisans");
		artisansPop->setSize((int)(demographic.ratio * oldPopulation * artisans / total));
		artisansPop->setCulture(demographic.culture);
		artisansPop->setReligion(demographic.religion);
		artisansPop->setLiteracy(demographic.literacy);
		artisansPop->setIdeology(demographic.reactionary, demographic.conservative, demographic.liberal);
		artisansPop->setIssues(demographic.issues);
		pops.push_back(artisansPop);
	}
	if (clergymen > 0)
	{
		V2Pop* clergymenPop = new V2Pop;
		clergymenPop->setType("clergymen");
		clergymenPop->setSize((int)(demographic.ratio * oldPopulation * clergymen / total));
		clergymenPop->setCulture(demographic.culture);
		clergymenPop->setReligion(demographic.religion);
		clergymenPop->setLiteracy(demographic.literacy);
		clergymenPop->setIdeology(demographic.reactionary, demographic.conservative, demographic.liberal);
		clergymenPop->setIssues(demographic.issues);
		pops.push_back(clergymenPop);
	}
	if (clerks > 0)
	{
		V2Pop* clerksPop = new V2Pop;
		clerksPop->setType("clerks");
		clerksPop->setSize((int)(demographic.ratio * oldPopulation * clerks / total));
		clerksPop->setCulture(demographic.culture);
		clerksPop->setReligion(demographic.religion);
		clerksPop->setLiteracy(demographic.literacy);
		clerksPop->setIdeology(demographic.reactionary, demographic.conservative, demographic.liberal);
		clerksPop->setIssues(demographic.issues);
		pops.push_back(clerksPop);
	}
	if (bureaucrats > 0)
	{
		V2Pop* bureaucratsPop = new V2Pop;
		bureaucratsPop->setType("bureaucrats");
		bureaucratsPop->setSize((int)(demographic.ratio * oldPopulation * bureaucrats / total));
		bureaucratsPop->setCulture(demographic.culture);
		bureaucratsPop->setReligion(demographic.religion);
		bureaucratsPop->setLiteracy(demographic.literacy);
		bureaucratsPop->setIdeology(demographic.reactionary, demographic.conservative, demographic.liberal);
		bureaucratsPop->setIssues(demographic.issues);
		pops.push_back(bureaucratsPop);
	}
	if (officers > 0)
	{
		V2Pop* officersPop = new V2Pop;
		officersPop->setType("officers");
		officersPop->setSize((int)(demographic.ratio * oldPopulation * officers / total));
		officersPop->setCulture(demographic.culture);
		officersPop->setReligion(demographic.religion);
		officersPop->setLiteracy(demographic.literacy);
		officersPop->setIdeology(demographic.reactionary, demographic.conservative, demographic.liberal);
		officersPop->setIssues(demographic.issues);
		pops.push_back(officersPop);
	}
	if (capitalists > 0)
	{
		V2Pop* capitalistsPop = new V2Pop;
		capitalistsPop->setType("capitalists");
		capitalistsPop->setSize((int)(demographic.ratio * oldPopulation * capitalists / total));
		capitalistsPop->setCulture(demographic.culture);
		capitalistsPop->setReligion(demographic.religion);
		capitalistsPop->setLiteracy(demographic.literacy);
		capitalistsPop->setIdeology(demographic.reactionary, demographic.conservative, demographic.liberal);
		capitalistsPop->setIssues(demographic.issues);
		pops.push_back(capitalistsPop);
	}
	if (aristocrats > 0)
	{
		V2Pop* aristocratsPop = new V2Pop;
		aristocratsPop->setType("aristocrats");
		aristocratsPop->setSize((int)(demographic.ratio * oldPopulation * aristocrats / total));
		aristocratsPop->setCulture(demographic.culture);
		aristocratsPop->setReligion(demographic.religion);
		aristocratsPop->setLiteracy(demographic.literacy);
		aristocratsPop->setIdeology(demographic.reactionary, demographic.conservative, demographic.liberal);
		aristocratsPop->setIssues(demographic.issues);
		pops.push_back(aristocratsPop);
	}
}


void V2Province::setPopConMil(string nationalCulture, vector<string> acceptedCultures, string nationalReligion, double nationalConModifier, double nationalMilModifier)
{
	for (vector<V2Pop*>::iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		double con = nationalConModifier;
		double mil = nationalMilModifier;
		// culture penalty - none if national culture
		if ( (*itr)->getCulture() != nationalCulture )
		{
			vector<string>::iterator accepted = std::find(acceptedCultures.begin(), acceptedCultures.end(), (*itr)->getCulture());
			if (accepted == acceptedCultures.end())
			{
				// non-accepted - 1 con + 1 mil
				mil += 1.0;
				con += 1.0;
			}
			else
			{
				// accepted - 0.5 con
				con += 0.5;
			}
		}
		// religion penalty - none if national religion
		if ( (*itr)->getReligion() != nationalReligion )
		{
			// TODO - should incorporate tolerance (not easily knowable), perhaps on a 0-1 or 0-2 scale
			mil += 1.0;
			con += 1.0;
		}
		// TODO - province decisions, province events, buildings, nationalism

		// cap at [0.0, 10.0]
		con = max(min(con, 10.0), 0.0);
		mil = max(min(mil, 10.0), 0.0);

		// make it so
		(*itr)->setConsciousness(con);
		(*itr)->setMilitancy(mil);
	}
}


int V2Province::getOldPopulation() const
{
	return oldPopulation;
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


// V2 requires 1000 for the first regiment and 3000 thereafter
// we require an extra 1/30 to stabilize the start of the game
static int getRequiredPopForRegimentCount(int count)
{
	if (count == 0) return 0;
	return (1033 + (count - 1) * 3100);
}


bool V2Province::growSoldierPop(int popID)
{
	for (vector<V2Pop*>::iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		if ( (*itr)->getID() == popID )
		{
			int currentlySupported = (*itr)->getSupportedRegimentCount();
			int growBy = getRequiredPopForRegimentCount(currentlySupported + 1) - (*itr)->getSize();
			if (growBy > 0)
			{
				// gotta grow; find a same-culture same-religion farmer/laborer to pull from
				int provincePop = getTotalPopulation();
				bool foundSourcePop = false;
				for (vector<V2Pop*>::iterator isrc = pops.begin(); isrc != pops.end(); ++isrc)
				{
					if ( (*isrc)->getType() == "farmers" || (*isrc)->getType() == "labourers" )
					{
						if ( (*isrc)->getCulture() == (*isrc)->getCulture() && (*isrc)->getReligion() == (*isrc)->getReligion() )
						{
							// don't let the farmer/labourer shrink beneath 10% of the province population
							if ( (*isrc)->getSize() - growBy > provincePop * 0.10 )
							{
								(*isrc)->setSize( (*isrc)->getSize() - growBy );
								(*itr)->setSize( (*itr)->getSize() + growBy );
								foundSourcePop = true;
								break;
							}
						}
					}
				}
				if (!foundSourcePop)
					return false;
			}
			(*itr)->setSupportedRegimentCount(++currentlySupported);
			return true;
		}
	}
	return false;
}


static bool PopSortBySizePredicate(const V2Pop* pop1, const V2Pop* pop2)
{
	return (pop1->getSize() > pop2->getSize());
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
			if (growSoldierPop( (*itr)->getID() )) // won't actually grow, but necessary to increment supported regiment count
				return (*itr)->getID();
		}
	}
	// try largest to smallest, trying to grow
	for (vector<V2Pop*>::iterator itr = spops.begin(); itr != spops.end(); ++itr)
	{
		if (growSoldierPop( (*itr)->getID() ))
			return (*itr)->getID();
	}

	// no suitable pops
	if (force)
		return spops[0]->getID();
	else
		return -1;
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


void V2Province::combinePops()
{
	vector<int> trashPops;
	for (vector<V2Pop*>::iterator lhs = pops.begin(); lhs != pops.end(); ++lhs)
	{
		vector<V2Pop*>::iterator rhs = lhs;
		for (++rhs; rhs != pops.end(); ++rhs)
		{
			if ( (*lhs)->canCombine(**rhs) )
			{
				(*lhs)->setSize( (*lhs)->getSize() + (*rhs)->getSize() );
				trashPops.push_back( (*rhs)->getID() );
			}
			if ( (*rhs)->getSize() < 1 )
				trashPops.push_back( (*rhs)->getID() );
		}
	}
	vector<V2Pop*> consolidatedPops;
	for (vector<V2Pop*>::iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		bool isTrashed = false;
		for (vector<int>::iterator titr = trashPops.begin(); titr != trashPops.end(); ++titr)
		{
			if ( (*itr)->getID() == (*titr))
				isTrashed = true;
		}
		if (!isTrashed)
			consolidatedPops.push_back(*itr);
	}
	pops.swap(consolidatedPops);
}


void V2Province::setFortLevel(int level)
{
	fortLevel = level;
}


void V2Province::setNavalBaseLevel(int level)
{
	navalBaseLevel = level;
}


void V2Province::setRailLevel(int level)
{
	railLevel = level;
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


bool PopSortForOutputPredicate(const V2Pop* pop1, const V2Pop* pop2)
{
	if (pop1->getType() != pop2->getType())
	{
		return pop1->getType() < pop2->getType();
	}
	return pop1->getID() < pop2->getID();
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
	if (fortLevel > 0)
	{
		fprintf(output, "	fort=\n");
		fprintf(output, "	{\n");
		fprintf(output, "		%f %f\n", (float)fortLevel, (float)fortLevel);
		fprintf(output, "	}\n");
	}
	if (navalBaseLevel > 0)
	{
		fprintf(output, "	naval_base=\n");
		fprintf(output, "	{\n");
		fprintf(output, "		%f %f\n", (float)navalBaseLevel, (float)navalBaseLevel);
		fprintf(output, "	}\n");
	}
	if (railLevel > 0)
	{
		fprintf(output, "	railroad=\n");
		fprintf(output, "	{\n");
		fprintf(output, "		%f %f\n", (float)railLevel, (float)railLevel);
		fprintf(output, "	}\n");
	}
	if (colonial)
	{
		fprintf(output, "	colonial=yes\n");
	}
	if (land)
	{
		if (owner == "")
		{
			sort(oldPops.begin(), oldPops.end(), PopSortForOutputPredicate);
			for (unsigned int i = 0; i < oldPops.size(); i++)
			{
				oldPops[i]->output(output);
			}
		}
		else
		{
			sort(pops.begin(), pops.end(), PopSortForOutputPredicate);
			for (unsigned int i = 0; i < pops.size(); i++)
			{
				pops[i]->output(output);
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
				if (oldPops[i]->getType() == "farmers")
				{
					fprintf(output, "				{\n");
					fprintf(output, "					province_pop_id=\n");
					fprintf(output, "					{\n");
					fprintf(output, "						province_id=%d\n", num);
					fprintf(output, "						index=%d\n", numFarmers);
					fprintf(output, "						type=8\n");
					fprintf(output, "					}\n");
					fprintf(output, "					count=%d\n", oldPops[i]->getSize());
					fprintf(output, "				}\n");
					numFarmers++;
				}
			}
			int numLabourers = 0;
			for (unsigned int i = 0; i < oldPops.size(); i++)
			{
				if (oldPops[i]->getType() == "labourers")
				{
					fprintf(output, "				{\n");
					fprintf(output, "					province_pop_id=\n");
					fprintf(output, "					{\n");
					fprintf(output, "						province_id=%d\n", num);
					fprintf(output, "						index=%d\n", numLabourers);
					fprintf(output, "						type=9\n");
					fprintf(output, "					}\n");
					fprintf(output, "					count=%d\n", oldPops[i]->getSize());
					fprintf(output, "				}\n");
					numLabourers++;
				}
			}
			int numSlaves = 0;
			for (unsigned int i = 0; i < oldPops.size(); i++)
			{
				if (oldPops[i]->getType() == "slaves")
				{
					fprintf(output, "				{\n");
					fprintf(output, "					province_pop_id=\n");
					fprintf(output, "					{\n");
					fprintf(output, "						province_id=%d\n", num);
					fprintf(output, "						index=%d\n", numSlaves);
					fprintf(output, "						type=11\n");
					fprintf(output, "					}\n");
					fprintf(output, "					count=%d\n", oldPops[i]->getSize());
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
				if (pops[i]->getType() == "farmers")
				{
					fprintf(output, "				{\n");
					fprintf(output, "					province_pop_id=\n");
					fprintf(output, "					{\n");
					fprintf(output, "						province_id=%d\n", num);
					fprintf(output, "						index=%d\n", numFarmers);
					fprintf(output, "						type=8\n");
					fprintf(output, "					}\n");
					fprintf(output, "					count=%d\n", pops[i]->getSize());
					fprintf(output, "				}\n");
					numFarmers++;
				}
			}
			int numLabourers = 0;
			for (unsigned int i = 0; i < pops.size(); i++)
			{
				if (pops[i]->getType() == "labourers")
				{
					fprintf(output, "				{\n");
					fprintf(output, "					province_pop_id=\n");
					fprintf(output, "					{\n");
					fprintf(output, "						province_id=%d\n", num);
					fprintf(output, "						index=%d\n", numLabourers);
					fprintf(output, "						type=9\n");
					fprintf(output, "					}\n");
					fprintf(output, "					count=%d\n", pops[i]->getSize());
					fprintf(output, "				}\n");
					numLabourers++;
				}
			}
			int numSlaves = 0;
			for (unsigned int i = 0; i < pops.size(); i++)
			{
				if (pops[i]->getType() == "slaves")
				{
					fprintf(output, "				{\n");
					fprintf(output, "					province_pop_id=\n");
					fprintf(output, "					{\n");
					fprintf(output, "						province_id=%d\n", num);
					fprintf(output, "						index=%d\n", numSlaves);
					fprintf(output, "						type=11\n");
					fprintf(output, "					}\n");
					fprintf(output, "					count=%d\n", pops[i]->getSize());
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