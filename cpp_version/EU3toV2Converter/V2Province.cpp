#include "V2Province.h"


void V2Province::init(Object* obj) {
	num = atoi(obj->getKey().c_str());

	vector<Object*> nameObj;
	nameObj = obj->getValue("name");
	if (nameObj.size() == 0)
	{
		name = "";
	}
	else
	{
		name = nameObj[0]->getLeaf();
	}

	vector<Object*> rgoObj;
	rgoObj = obj->getValue("rgo");
	if (rgoObj.size() == 0)
	{
		land = false;
	}
	else
	{
		land = true;
		vector<Object*> goodsObj= rgoObj[0]->getValue("goods_type");
		rgoType = goodsObj[0]->getLeaf();
		lifeRating = atoi( obj->getValue("life_rating")[0]->getLeaf().c_str() );
	}

	oldPopulation = 0;
	vector<Object*> leaves = obj->getLeaves();
	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		string key = leaves[i]->getKey();
		if ( ( key == "aristocrats") || ( key == "artisans") || ( key == "bureaucrats") || ( key == "") || ( key == "clergymen") || ( key == "clerks") || ( key == "craftsmen") || ( key == "farmers") || ( key == "labourers") || ( key == "officers") || ( key == "slaves") || ( key == "soldiers") )
		{
			oldPopulation += atoi( (leaves[i]->getValue("size"))[0]->getLeaf().c_str() );
		}
	}
}


int V2Province::getNum()
{
	return num;
}


void V2Province::setOwner(string newOwner)
{
	owner = newOwner;
}


void V2Province::setColonial(bool isIt)
{
	colonial = isIt;
}


void V2Province::setCulture(string newCulture)
{
	culture = newCulture;
}


bool V2Province::isColonial()
{
	return colonial;
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

	if ( (rgoType == "\"cattle\"") || (rgoType == "\"coffee\"") || (rgoType == "\"cotton\"") || (rgoType == "\"dye\"") || (rgoType == "\"fish\"") || (rgoType == "\"fruit\"") || (rgoType == "\"grain\"") || (rgoType == "\"opium\"") || (rgoType == "\"silk\"") || (rgoType == "\"tea\"") || (rgoType == "\"tobacco\"") || (rgoType == "\"wool\"") )
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
		farmersPop.setReligion("animist");
		pops.push_back(farmersPop);
	}
	if (labourers > 0)
	{
		V2Pop labourersPop;
		labourersPop.setType("labourers");
		labourersPop.setSize(oldPopulation * labourers / total);
		labourersPop.setCulture(culture);
		labourersPop.setReligion("animist");
		pops.push_back(labourersPop);
	}
	if (slaves > 0)
	{
		V2Pop slavesPop;
		slavesPop.setType("slaves");
		slavesPop.setSize(oldPopulation * slaves / total);
		slavesPop.setCulture(culture);
		slavesPop.setReligion("animist");
		pops.push_back(slavesPop);
	}
	if (soldiers > 0)
	{
		V2Pop soldiersPop;
		soldiersPop.setType("soldiers");
		soldiersPop.setSize(oldPopulation * soldiers / total);
		soldiersPop.setCulture(culture);
		soldiersPop.setReligion("animist");
		pops.push_back(soldiersPop);
	}
	if (craftsmen > 0)
	{
		V2Pop craftsmenPop;
		craftsmenPop.setType("craftsmen");
		craftsmenPop.setSize(oldPopulation * craftsmen / total);
		craftsmenPop.setCulture(culture);
		craftsmenPop.setReligion("animist");
		pops.push_back(craftsmenPop);
	}
	if (artisans > 0)
	{
		V2Pop artisansPop;
		artisansPop.setType("artisans");
		artisansPop.setSize(oldPopulation * artisans / total);
		artisansPop.setCulture(culture);
		artisansPop.setReligion("animist");
		pops.push_back(artisansPop);
	}
	if (clergymen > 0)
	{
		V2Pop clergymenPop;
		clergymenPop.setType("clergymen");
		clergymenPop.setSize(oldPopulation * clergymen / total);
		clergymenPop.setCulture(culture);
		clergymenPop.setReligion("animist");
		pops.push_back(clergymenPop);
	}
	if (clerks > 0)
	{
		V2Pop clerksPop;
		clerksPop.setType("clerks");
		clerksPop.setSize(oldPopulation * clerks / total);
		clerksPop.setCulture(culture);
		clerksPop.setReligion("animist");
		pops.push_back(clerksPop);
	}
	if (bureaucrats > 0)
	{
		V2Pop bureaucratsPop;
		bureaucratsPop.setType("bureaucrats");
		bureaucratsPop.setSize(oldPopulation * bureaucrats / total);
		bureaucratsPop.setCulture(culture);
		bureaucratsPop.setReligion("animist");
		pops.push_back(bureaucratsPop);
	}
	if (officers > 0)
	{
		V2Pop officersPop;
		officersPop.setType("officers");
		officersPop.setSize(oldPopulation * officers / total);
		officersPop.setCulture(culture);
		officersPop.setReligion("animist");
		pops.push_back(officersPop);
	}
	if (capitalists > 0)
	{
		V2Pop capitalistsPop;
		capitalistsPop.setType("capitalists");
		capitalistsPop.setSize(oldPopulation * capitalists / total);
		capitalistsPop.setCulture(culture);
		capitalistsPop.setReligion("animist");
		pops.push_back(capitalistsPop);
	}
	if (aristocrats > 0)
	{
		V2Pop aristocratsPop;
		aristocratsPop.setType("aristocrats");
		aristocratsPop.setSize(oldPopulation * aristocrats / total);
		aristocratsPop.setCulture(culture);
		aristocratsPop.setReligion("animist");
		pops.push_back(aristocratsPop);
	}
}



void V2Province::output(FILE* output)
{
	fprintf(output, "%d=\n", num);
	fprintf(output, "{\n");
	fprintf(output, "	name=%s\n", name.c_str());
	if (land)
	{
		if (owner != "")
		{
			fprintf(output, "	owner=\"%s\"\n", owner.c_str());
			fprintf(output, "	controller=\"%s\"\n", owner.c_str());
			fprintf(output, "	core=\"%s\"\n", owner.c_str());
		}
	}
	fprintf(output, "	garrison=100.000\n");
	if (land)
	{
		for (unsigned int i = 0; i < pops.size(); i++)
		{
			pops[i].output(output);
		}
		fprintf(output, "	rgo=\n");
		fprintf(output, "	{\n");
		fprintf(output, "		goods_type=%s\n", rgoType.c_str());
		fprintf(output, "	}\n");
		fprintf(output, "	life_rating=%d\n", lifeRating);
	}
	fprintf(output, "}\n");
}