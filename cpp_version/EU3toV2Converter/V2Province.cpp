#include "V2Province.h"
#include "Log.h"


void V2Province::init(int newNumber)
{
	num				= newNumber;
	name				= "";
	land				= false;
	oldPopulation	= 0;
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
	cores.push_back(newCore);
}


void V2Province::setColonial(bool isIt)
{
	colonial = isIt;
}


void V2Province::setCulture(string newCulture)
{
	culture = newCulture;
}


void V2Province::setReligion(string newReligion)
{
	religion = newReligion;
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


void V2Province::createPops(EU3Province* oldProvince, EU3Country* oldCountry)
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
		farmersPop.setSize(oldPopulation * farmers / total);
		farmersPop.setCulture(culture);
		farmersPop.setReligion(religion);
		pops.push_back(farmersPop);
	}
	if (labourers > 0)
	{
		V2Pop labourersPop;
		labourersPop.setType("labourers");
		labourersPop.setSize(oldPopulation * labourers / total);
		labourersPop.setCulture(culture);
		labourersPop.setReligion(religion);
		pops.push_back(labourersPop);
	}
	if (slaves > 0)
	{
		V2Pop slavesPop;
		slavesPop.setType("slaves");
		slavesPop.setSize(oldPopulation * slaves / total);
		slavesPop.setCulture(culture);
		slavesPop.setReligion(religion);
		pops.push_back(slavesPop);
	}
	if (soldiers > 0)
	{
		V2Pop soldiersPop;
		soldiersPop.setType("soldiers");
		soldiersPop.setSize(oldPopulation * soldiers / total);
		soldiersPop.setCulture(culture);
		soldiersPop.setReligion(religion);
		pops.push_back(soldiersPop);
	}
	if (craftsmen > 0)
	{
		V2Pop craftsmenPop;
		craftsmenPop.setType("craftsmen");
		craftsmenPop.setSize(oldPopulation * craftsmen / total);
		craftsmenPop.setCulture(culture);
		craftsmenPop.setReligion(religion);
		pops.push_back(craftsmenPop);
	}
	if (artisans > 0)
	{
		V2Pop artisansPop;
		artisansPop.setType("artisans");
		artisansPop.setSize(oldPopulation * artisans / total);
		artisansPop.setCulture(culture);
		artisansPop.setReligion(religion);
		pops.push_back(artisansPop);
	}
	if (clergymen > 0)
	{
		V2Pop clergymenPop;
		clergymenPop.setType("clergymen");
		clergymenPop.setSize(oldPopulation * clergymen / total);
		clergymenPop.setCulture(culture);
		clergymenPop.setReligion(religion);
		pops.push_back(clergymenPop);
	}
	if (clerks > 0)
	{
		V2Pop clerksPop;
		clerksPop.setType("clerks");
		clerksPop.setSize(oldPopulation * clerks / total);
		clerksPop.setCulture(culture);
		clerksPop.setReligion(religion);
		pops.push_back(clerksPop);
	}
	if (bureaucrats > 0)
	{
		V2Pop bureaucratsPop;
		bureaucratsPop.setType("bureaucrats");
		bureaucratsPop.setSize(oldPopulation * bureaucrats / total);
		bureaucratsPop.setCulture(culture);
		bureaucratsPop.setReligion(religion);
		pops.push_back(bureaucratsPop);
	}
	if (officers > 0)
	{
		V2Pop officersPop;
		officersPop.setType("officers");
		officersPop.setSize(oldPopulation * officers / total);
		officersPop.setCulture(culture);
		officersPop.setReligion(religion);
		pops.push_back(officersPop);
	}
	if (capitalists > 0)
	{
		V2Pop capitalistsPop;
		capitalistsPop.setType("capitalists");
		capitalistsPop.setSize(oldPopulation * capitalists / total);
		capitalistsPop.setCulture(culture);
		capitalistsPop.setReligion(religion);
		pops.push_back(capitalistsPop);
	}
	if (aristocrats > 0)
	{
		V2Pop aristocratsPop;
		aristocratsPop.setType("aristocrats");
		aristocratsPop.setSize(oldPopulation * aristocrats / total);
		aristocratsPop.setCulture(culture);
		aristocratsPop.setReligion(religion);
		pops.push_back(aristocratsPop);
	}
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
			for (unsigned int i = 0; i < oldPops.size(); i++)
			{
				oldPops[i].output(output);
			}
		}
		else
		{
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
			for (unsigned int i = 0; i < oldPops.size(); i++)
			{
				int numFarmers = 0;
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
			for (unsigned int i = 0; i < oldPops.size(); i++)
			{
				int numLabourers = 0;
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
			for (unsigned int i = 0; i < oldPops.size(); i++)
			{
				int numSlaves = 0;
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
			for (unsigned int i = 0; i < pops.size(); i++)
			{
				int numFarmers = 0;
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
			for (unsigned int i = 0; i < pops.size(); i++)
			{
				int numLabourers = 0;
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
			for (unsigned int i = 0; i < pops.size(); i++)
			{
				int numSlaves = 0;
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
		fprintf(output, "		goods_type=\"%s\"\n", rgoType.c_str());
		fprintf(output, "	}\n");
		fprintf(output, "	life_rating=%d\n", lifeRating);
	}
	fprintf(output, "}\n");
}