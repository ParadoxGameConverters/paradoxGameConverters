#include "EU3Country.h"
#include "..\Log.h"
#include "..\Parsers\Parser.h"
#include "..\Parsers\Object.h"
#include "..\Configuration.h"
#include "..\Date.h"
#include "..\CK2World\CK2Province.h"
#include "..\CK2World\CK2Title.h"
#include "..\CK2World\CK2History.h"
#include "..\CK2World\CK2Character.h"
#include "..\CK2World\CK2Barony.h"
#include "..\CK2World\CK2Techs.h"
#include "..\CK2World\CK2Religion.h"
#include "..\CK2World\CK2Army.h"
#include "EU3Ruler.h"
#include "EU3Advisor.h"
#include "EU3History.h"
#include "EU3Province.h"
#include "EU3World.h"
#include "EU3Tech.h"
#include "EU3Diplomacy.h"
#include "EU3Army.h"
#include "EU3Navy.h"
#include <fstream>
using namespace std;


EU3Country::EU3Country(EU3World* world, string _tag, string newHistoryFile, date startDate, const EU3Tech* techData)
{
	src				= NULL;
	liege				= NULL;
	vassals.clear();
	provinces.clear();
	learningScore	= 0.0;
	absorbScore		= INT_MAX;

	tag			= _tag;
	historyFile	= newHistoryFile;

	// Parse history file
	Object* obj;
	obj = doParseFile( (Configuration::getEU3Path() + "/history/countries/" + historyFile).c_str() );
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getEU3Path() + "/history/countries/" + historyFile).c_str());
		printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "/history/countries/" + historyFile).c_str());
		exit(-1);
	}

	// Set objects from top of history file
	vector<Object*> govLeaves = obj->getValue("government");
	if (govLeaves.size() > 0)
	{
		government = govLeaves[0]->getLeaf();
	}
	else
	{
		government = "";
	}

	vector<Object*> religionLeaves = obj->getValue("religion");
	if (religionLeaves.size() > 0)
	{
		religion = religionLeaves[0]->getLeaf();
	}
	else
	{
		religion = "";
	}

	vector<Object*> primaryCultureLeaves = obj->getValue("primary_culture");
	if (primaryCultureLeaves.size() > 0)
	{
		primaryCulture = primaryCultureLeaves[0]->getLeaf();
	}
	else
	{
		primaryCulture = "";
	}

	acceptedCultures.clear();
	vector<Object*> acceptedCulturesLeaves = obj->getValue("add_accepted_culture");
	for (vector<Object*>::iterator i = acceptedCulturesLeaves.begin(); i < acceptedCulturesLeaves.end(); i++)
	{
		acceptedCultures.push_back( (*i)->getLeaf() );
	}

	monarch	= NULL;
	heir		= NULL;
	regent	= NULL;

	history.clear();
	previousMonarchs.clear();

	vector<Object*> techLeaves = obj->getValue("technology_group");
	if (techLeaves.size() > 0)
	{
		techGroup = techLeaves[0]->getLeaf();
	}
	else
	{
		techGroup = "";
	}
	governmentTech	= techData->getGovernmentTech(techGroup);
	productionTech	= techData->getProductionTech(techGroup);
	tradeTech		= techData->getTradeTech(techGroup);
	navalTech		= techData->getNavalTech(techGroup);
	landTech			= techData->getLandTech(techGroup);
	governmentTechInvestment	= 0.0f;
	productionTechInvestment	= 0.0f;
	tradeTechInvestment			= 0.0f;
	navalTechInvestment			= 0.0f;
	landTechInvestment			= 0.0f;
	agreements.clear();

	armies.clear();
	navies.clear();
	manpower = 0.0F;
	// Todo: Set the prefferred unit types replace with something better later
	if(techGroup == "western")
	{
		infantry = "western_medieval_infantry";
		cavalry = "western_medieval_knights";
		bigShip = "carrack";
		galley = "galley";
		transport = "cog";
	}
	else if(techGroup == "eastern")
	{
		infantry="eastern_medieval_infantry";
		cavalry="eastern_knights";
		bigShip = "carrack";
		galley = "galley";
		transport = "cog";
	}
	else if(techGroup == "ottoman")
	{
		infantry = "ottoman_yaya";
		cavalry = "ottoman_musellem";
		bigShip = "carrack";
		galley = "galley";
		transport = "cog";
	}
	else if(techGroup == "muslim")
	{
		infantry = "mamluk_archer";
		cavalry = "muslim_cavalry_archers";
		bigShip = "carrack";
		galley = "galley";
		transport = "cog";
	}
	else if(techGroup == "indian")
	{
		infantry = "indian_footsoldier";
		cavalry = "indian_archers";
		bigShip = "carrack";
		galley = "galley";
		transport = "cog";
	}
	else if(techGroup == "chinese")
	{
		infantry = "chinese_longspear";
		cavalry = "eastern_bow";
		bigShip = "carrack";
		galley = "galley";
		transport = "cog";
	}
	else if(techGroup == "sub_saharan")
	{
		infantry = "african_spearmen";
		transport = "cog";
	}
	else if(techGroup == "new_world")
	{
		infantry = "south_american_spearmen";
	}
	else if(techGroup == "nomad_group")
	{
		infantry = "mongol_bow";
		cavalry = "mongol_swarm";
		bigShip = "carrack";
		galley = "galley";
		transport = "cog";
	}

	vector<Object*> capitalObj = obj->getValue("capital");
	if (capitalObj.size() > 0)
	{
		capital = atoi( capitalObj[0]->getLeaf().c_str() );
	}
	else
	{
		capital = 0;
	}

	prestige				= 0.0;
	stability				= 1;
	stabilityInvestment	= 0.0f;

	estimatedIncome		= 0.0f;
	estimatedTax			= 0.0f;
	estimatedGold			= 0.0f;
	estimatedProduction	= 0.0f;
	estimatedTolls			= 0.0f;
	
	vector<Object*> daimyoObj = obj->getValue("daimyo");
	if (daimyoObj.size() > 0)
	{
		string leaf = daimyoObj[0]->getLeaf();
		if (leaf == "emperor")
		{
			daimyo				= true;
			japaneseEmperor	= true;
			world->setJapaneseEmperor(this);
		}
		else
		{
			daimyo				= true;
			japaneseEmperor	= false;
			world->addDamiyo(this);
		}
	}
	else
	{
		daimyo				= false;
		japaneseEmperor	= false;
	}
	elector = false;

	// update items based on history
	vector<Object*> objectList = obj->getLeaves();
	for (unsigned int i = 0; i < objectList.size(); i++)
	{
		string key = objectList[i]->getKey();
		if (key[0] == '1')
		{
			date histDate(key);
			if (histDate <= startDate)
			{
				EU3History* newHistory = new EU3History(histDate);

				vector<Object*> newMonarchObj = objectList[i]->getValue("monarch");
				if (newMonarchObj.size() > 0)
				{
					monarch = new EU3Ruler(newMonarchObj[0]);
					previousMonarchs.push_back(monarch);
					newHistory->monarch = monarch;
				}

				vector<Object*> newHeirObj = objectList[i]->getValue("heir");
				if (newHeirObj.size() > 0)
				{
					heir = new EU3Ruler(newHeirObj[0]);
					newHistory->heir = heir;
				}

				vector<Object*> govLeaves = objectList[i]->getValue("government");
				if (govLeaves.size() > 0)
				{
					government = govLeaves[0]->getLeaf();
					newHistory->government = government;
				}

				vector<Object*> religionLeaves = objectList[i]->getValue("religion");
				if (religionLeaves.size() > 0)
				{
					religion = religionLeaves[0]->getLeaf();
					newHistory->religion = religion;
				}

				vector<Object*> primaryCultureLeaves = objectList[i]->getValue("primary_culture");
				if (primaryCultureLeaves.size() > 0)
				{
					primaryCulture = primaryCultureLeaves[0]->getLeaf();
					newHistory->primaryCulture = primaryCulture;
				}

				vector<Object*> acceptedCulturesLeaves = objectList[i]->getValue("add_accepted_culture");
				for (vector<Object*>::iterator j = acceptedCulturesLeaves.begin(); j < acceptedCulturesLeaves.end(); j++)
				{
					acceptedCultures.push_back( (*j)->getLeaf() );
					newHistory->acceptedCultures.push_back( (*j)->getLeaf() );
				}

				vector<Object*> techLeaves = objectList[i]->getValue("technology_group");
				if (techLeaves.size() > 0)
				{
					techGroup = techLeaves[0]->getLeaf();
					newHistory->techGroup = techGroup;
				}

				vector<Object*> capitalObj = objectList[i]->getValue("capital");
				if (capitalObj.size() > 0)
				{
					capital = atoi( capitalObj[0]->getLeaf().c_str() );
				}

				vector<Object*> shogunObj = objectList[i]->getValue("shogun");
				if(shogunObj.size() > 0)
				{
					world->setShogun(this);
					double shogunPower = atof( shogunObj[0]->getLeaf().c_str() );
					world->setShogunPower(shogunPower);
					newHistory->shogunPower = shogunPower;
				}

				history.push_back(newHistory);
			}
		}
	}
	if (previousMonarchs.size() > 0)
	{
		previousMonarchs.pop_back();
	}

	merchants		= 2.0f;
	colonists		= 2.0f;
	diplomats		= 2.0f;
	missionaries	= 2.0f;
	spies				= 2.0f;
	magistrates		= 2.0f;
}


EU3Country::EU3Country(CK2Title* _src, const religionMapping& religionMap, const cultureMapping& cultureMap, const inverseProvinceMapping& inverseProvinceMap)
{
	src = _src;
	src->setDstCountry(this);
	liege = NULL;
	vassals.clear();
	provinces.clear();
	cores.clear();
	advisors.clear();
	learningScore = 0.0F;
	absorbScore = 0;

	tag			= "";
	historyFile	= "";
	government	= "";
	monarch		= NULL;
	heir			= NULL;
	regent		= NULL;
	history.clear();
	previousMonarchs.clear();

	religion = "";
	CK2Religion* oldReligion = src->getLastHolder()->getReligion();
	if (oldReligion)
	{
		religionMapping::const_iterator religionItr = religionMap.find(oldReligion->getName());
		if (religionItr != religionMap.end())
		{
			religion = religionItr->second;
		}
	}

	map<string, int> cultureWeights;
	src->getCultureWeights(cultureWeights, cultureMap);
	int highestWeight = 0;
	primaryCulture		= "";
	for (map<string, int>::iterator cultureItr = cultureWeights.begin(); cultureItr != cultureWeights.end(); cultureItr++)
	{
		if (cultureItr->second > highestWeight)
		{
			primaryCulture	= cultureItr->first;
			highestWeight	= cultureItr->second;
		}
	}

	acceptedCultures.clear();

	techGroup = "";
	governmentTech					= 0.0f;
	productionTech					= 0.0f;
	tradeTech						= 0.0f;
	navalTech						= 0.0f;
	landTech							= 0.0f;
	governmentTechInvestment	= 0.0f;
	productionTechInvestment	= 0.0f;
	tradeTechInvestment			= 0.0f;
	navalTechInvestment			= 0.0f;
	landTechInvestment			= 0.0f;
	agreements.clear();
	relations.clear();

	CK2Province* srcCapital = src->getLastHolder()->getCapital();
	if (srcCapital != NULL)
	{
		provinceMapping::const_iterator capitalItr = inverseProvinceMap.find( srcCapital->getNumber() );
		if (capitalItr != inverseProvinceMap.end())
		{
			capital = capitalItr->second[0];
		}
		else
		{
			capital = 0;
		}
	}
	else
	{
		capital = 0;
	}

	prestige				= 0.0;
	stability				= 1;
	stabilityInvestment	= 0.0f;
	estimatedIncome		= 0.0f;
	estimatedTax			= 0.0f;
	estimatedGold			= 0.0f;
	estimatedProduction	= 0.0f;
	estimatedTolls			= 0.0f;

	daimyo				= false;
	japaneseEmperor	= false;
	elector				= false;

	armies.clear();
	navies.clear();
	manpower	= 0.0F;
	// todo: replace with something better
	infantry = "western_medieval_infantry";
	cavalry = "western_medieval_knights";
	bigShip = "carrack";
	galley = "galley";
	transport = "cog";

	merchants		= 2.0f;
	colonists		= 2.0f;
	diplomats		= 2.0f;
	missionaries	= 2.0f;
	spies				= 2.0f;
	magistrates		= 2.0f;

	date ascensionDate;
	vector<CK2History*> oldHistory = src->getHistory();
	for (unsigned int i = 0; i < oldHistory.size(); i++)
	{
		EU3History* newHistory = new EU3History(oldHistory[i]);
		history.push_back(newHistory);

		if (newHistory->regent != NULL)
		{
			previousMonarchs.push_back(newHistory->regent);
		}
		else if (newHistory->monarch != NULL)
		{
			previousMonarchs.push_back(newHistory->monarch);
		}

		if ( (oldHistory[i]->getHolder() != NULL) && (src->getHolder() == oldHistory[i]->getHolder()) )
		{
			EU3Ruler* newRegent = newHistory->regent;
			if (newRegent != NULL)
			{
				monarch			= newRegent;
				heir				= newHistory->heir;
				ascensionDate	= newHistory->when;
			}
			else
			{
				monarch			= newHistory->monarch;
				ascensionDate	= newHistory->when;
			}
		}
	}
	for (vector<EU3Ruler*>::iterator i = previousMonarchs.begin(); i != previousMonarchs.end(); i++)
	{
		for(vector<EU3Ruler*>::iterator j = previousMonarchs.begin(); j != i; j++)
		{
			if ( (*i)->getName() == (*j)->getName())
			{
				(*i)->setRegnalNum( (*j)->getRegnalNum() + 1 );
			}
		}
	}
	if (previousMonarchs.size() > 0)
	{
		previousMonarchs.pop_back();
	}

	if (heir == NULL)
	{
		CK2Character* newHeir = src->getHeir();
		if (newHeir != NULL)
		{
			heir = new EU3Ruler(newHeir);

			date when = newHeir->getBirthDate();
			if (when < ascensionDate)
			{
				when = ascensionDate;
			}

			EU3History* newHistory = new EU3History(when);
			newHistory->heir = heir;
			history.push_back(newHistory);
		}
	}
}


void EU3Country::output(FILE* output)
{
	fprintf(output, "%s=\n", tag.c_str());
	fprintf(output, "{\n");
	fprintf(output, "\thistory=\n");
	fprintf(output, "\t{\n");
	if (daimyo && japaneseEmperor)
	{
		fprintf(output, "\t\tdaimyo=yes\n");
	}
	else if (daimyo)
	{
		fprintf(output, "\t\tdaimyo=no\n");
	}
	if (government != "")
	{
		fprintf(output, "\t\tgovernment=%s\n", government.c_str());
	}
	else
	{
		log("\tWarning: No government for %s\n", tag.c_str());
	}
	if (techGroup != "")
	{
		fprintf(output, "\t\ttechnology_group=%s\n", techGroup.c_str());
	}
	else
	{
		fprintf(output, "\t\ttechnology_group=new_world\n");
		log("\tWarning: No tech group for %s (defaulting to new world)\n", tag.c_str());
	}
	if (primaryCulture != "")
	{
		fprintf(output, "\t\tprimary_culture=%s\n", primaryCulture.c_str());
	}
	else
	{
		log("\tWarning: No primary culture for %s\n", tag.c_str());
	}
	for (unsigned int i = 0; i < acceptedCultures.size(); i++)
	{
		fprintf(output, "\t\tadd_accepted_culture=%s\n", acceptedCultures[i].c_str());
	}
	if (religion != "")
	{
		fprintf(output, "\t\treligion=%s\n", religion.c_str());
	}
	else
	{
		log("\tWarning: No religion for %s\n", tag.c_str());
	}
	if (capital != 0)
	{
		fprintf(output, "\t\tcapital=%d\n", capital);
	}
	else
	{
		log("\tWarning: No capital for %s\n", tag.c_str());
	}
	for (unsigned int i = 0; i < history.size(); i++)
	{
		history[i]->output(output);
	}
	fprintf(output, "\t}\n");
	if (capital != 0)
	{
		fprintf(output, "\tcapital=%d\n", capital);
	}
	if (primaryCulture != "")
	{
		fprintf(output, "\tprimary_culture=%s\n", primaryCulture.c_str());
	}
	for (unsigned int i = 0; i < acceptedCultures.size(); i++)
	{
		fprintf(output, "\taccepted_culture=%s\n", acceptedCultures[i].c_str());
	}
	if (religion != "")
	{
		fprintf(output, "\treligion=%s\n", religion.c_str());
	}
	if (techGroup != "")
	{
		fprintf(output, "\ttechnology_group=%s\n", techGroup.c_str());
	}
	else
	{
		fprintf(output, "\ttechnology_group=new_world\n");
	}
	fprintf(output, "\ttechnology=\n");
	fprintf(output, "\t{\n");
	fprintf(output, "\t\tland_tech={%d %f}\n", (int)landTech, landTechInvestment);
	fprintf(output, "\t\tnaval_tech={%d %f}\n", (int)navalTech, navalTechInvestment);
	fprintf(output, "\t\ttrade_tech={%d %f}\n", (int)tradeTech, tradeTechInvestment);
	fprintf(output, "\t\tproduction_tech={%d %f}\n", (int)productionTech, productionTechInvestment);
	fprintf(output, "\t\tgovernment_tech={%d %f}\n", (int)governmentTech, governmentTechInvestment);
	fprintf(output, "\t}\n");
	if (elector)
	{
		fprintf(output, "elector=yes\n");
		fprintf(output, "last_hre_vote=\"1.1.1\"\n");
	}
	fprintf(output, "\tprecise_prestige=%f\n", prestige);
	fprintf(output, "\tstability=%f\n", (double)stability);
	fprintf(output, "\tstability_investment=%f\n", stabilityInvestment);
	fprintf(output, "\ttreasury=%f\n", estimatedIncome * 13); // TODO: put in real money calculations
	fprintf(output, "\tcurrent_income=0.000\n");
	fprintf(output, "\testimated_monthly_income=%f\n", estimatedIncome);
	fprintf(output, "\tinflation=0.000\n");
	fprintf(output, "\tlast_bankrupt=\"1.1.1\"\n");
	fprintf(output, "\twartax=\"1.1.1\"\n");
	fprintf(output, "\twar_exhaustion=0.000\n");
	fprintf(output, "\tland_maintenance=1.000\n");
	fprintf(output, "\tnaval_maintenance=1.000\n");
	fprintf(output, "\tcolonial_maintenance=1.000\n");
	fprintf(output, "\tmissionary_maintenance=1.000\n");
	fprintf(output, "\tdistribution=\n");
	fprintf(output, "\t{\n");
	fprintf(output, "\t\t0.000 0.170 0.166 0.166 0.166 0.166 0.166\n");
	fprintf(output, "\t}\n");
	fprintf(output, "\tledger=\n");
	fprintf(output, "\t{\n");
	fprintf(output, "\t\tincome=\n");
	fprintf(output, "\t\t{\n");
	fprintf(output, "\t\t\t%f 0.000 0.000 %f %f 0.000 0.000 0.000 0.000 0.000 0.000 0.000 %f 0.000 0.000 0.000 0.000\n", estimatedTax, estimatedGold, estimatedProduction, estimatedTolls);
	fprintf(output, "\t\t}\n");
	fprintf(output, "\t\texpense=\n");
	fprintf(output, "\t\t{\n");
	fprintf(output, "\t\t\t0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000\n");
	fprintf(output, "\t\t}\n");
	fprintf(output, "\t\tthismonthincome=\n");
	fprintf(output, "\t\t{\n");
	fprintf(output, "\t\t\t0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000\n");
	fprintf(output, "\t\t}\n");
	fprintf(output, "\t\tthismonthexpense=\n");
	fprintf(output, "\t\t{\n");
	fprintf(output, "\t\t\t0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000\n");
	fprintf(output, "\t\t}\n");
	fprintf(output, "\t\tlastmonthincometable=\n");
	fprintf(output, "\t\t{\n");
	fprintf(output, "\t\t\t%f 0.000 0.000 %f %f 0.000 0.000 0.000 0.000 0.000 0.000 0.000 %f 0.000 0.000 0.000 0.000\n", estimatedTax, estimatedGold, estimatedProduction, estimatedTolls);
	fprintf(output, "\t\t}\n");
	fprintf(output, "\t\tlastmonthexpensetable=\n");
	fprintf(output, "\t\t{\n");
	fprintf(output, "\t\t\t0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000\n");
	fprintf(output, "\t\t}\n");
	fprintf(output, "\t\tlastyearincome=\n");
	fprintf(output, "\t\t{\n");
	fprintf(output, "\t\t\t0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000\n");
	fprintf(output, "\t\t}\n");
	fprintf(output, "\t\tlastyearexpense=\n");
	fprintf(output, "\t\t{\n");
	fprintf(output, "\t\t\t0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000\n");
	fprintf(output, "\t\t}\n");
	fprintf(output, "\t\tlastmonthincome=%f\n", estimatedIncome);
	fprintf(output, "\t\tlastmonthexpense=0.000\n");
	fprintf(output, "\t}\n");
	fprintf(output, "\tbadboy=0.000\n");
	fprintf(output, "\tlegitimacy=1.000\n");
	if (government != "")
	{
		fprintf(output, "\tgovernment=%s\n", government.c_str());
	}
	else
	{
		fprintf(output, "\tgovernment=tribal_despotism\n");
	}
	fprintf(output, "\tcolonists=%f\n", colonists);
	fprintf(output, "\tmerchants=%f\n", merchants);
	fprintf(output, "\tmissionaries=%f\n", missionaries);
	fprintf(output, "\tspies=%f\n", spies);
	fprintf(output, "\tdiplomats=%f\n", diplomats);
	fprintf(output, "\tofficials=%f\n", magistrates);
	fprintf(output, "\tmanpower=%f\n", manpower);
	if(infantry != "")
	{
		fprintf(output, "\tinfantry=\"%s\"\n", infantry.c_str());
	}
	if(cavalry != "")
	{
		fprintf(output, "\tcavalry=\"%s\"\n", cavalry.c_str());
	}
	if(bigShip != "")
	{
		fprintf(output, "\tbig_ship=\"%s\"\n", bigShip.c_str());
	}
	if(galley != "")
	{
		fprintf(output, "\tgalley=\"%s\"\n", galley.c_str());
	}
	if(transport != "")
	{
		fprintf(output, "\ttransport=\"%s\"\n", transport.c_str());
	}
	for (unsigned int i = 0; i < armies.size(); i++)
	{
		armies[i]->output(output);
	}
	for (unsigned int i = 0; i < navies.size(); i++)
	{
		navies[i]->output(output);
	}
	for (map<EU3Country*, int>::const_iterator itr = relations.begin(); itr != relations.end(); ++itr)
	{
		fprintf(output, "\t%s=\n", itr->first->getTag().c_str());
		fprintf(output, "\t{\n");
		fprintf(output, "\t\tvalue=%d\n", itr->second);
		fprintf(output, "\t}\n");
	}
	if (regent != NULL)
	{
		fprintf(output, "\tmonarch=\n");
		fprintf(output, "\t{\n");
		fprintf(output, "\t\tid=%d\n", regent->getID());
		fprintf(output, "\t\ttype=37\n");
		fprintf(output, "\t}\n");
	}
	else if (monarch != NULL)
	{
		fprintf(output, "\tmonarch=\n");
		fprintf(output, "\t{\n");
		fprintf(output, "\t\tid=%d\n", monarch->getID());
		fprintf(output, "\t\ttype=37\n");
		fprintf(output, "\t}\n");
	}
	for (unsigned int i = 0; i < previousMonarchs.size(); i++)
	{
		fprintf(output, "\tprevious_monarch=\n");
		fprintf(output, "\t{\n");
		fprintf(output, "\t\tid=%d\n", previousMonarchs[i]->getID());
		fprintf(output, "\t\ttype=37\n");
		fprintf(output, "\t}\n");
	}
	if (regent != NULL)
	{
		fprintf(output, "\their=\n");
		fprintf(output, "\t{\n");
		fprintf(output, "\t\tid=%d\n", monarch->getID());
		fprintf(output, "\t\ttype=37\n");
		fprintf(output, "\t}\n");
	}
	else if (heir != NULL)
	{
		fprintf(output, "\their=\n");
		fprintf(output, "\t{\n");
		fprintf(output, "\t\tid=%d\n", heir->getID());
		fprintf(output, "\t\ttype=37\n");
		fprintf(output, "\t}\n");
	}
	fprintf(output, "}\n");
}


void EU3Country::determineLearningScore()
{
	int numBaronies = 0;
	for (vector<EU3Province*>::iterator provinceItr = provinces.begin(); provinceItr < provinces.end(); provinceItr++)
	{
		vector<CK2Province*> srcProvinces = (*provinceItr)->getSrcProvinces();
		for (vector<CK2Province*>::iterator srcItr = srcProvinces.begin(); srcItr < srcProvinces.end(); srcItr++)
		{
			vector<CK2Barony*> baronies = (*srcItr)->getBaronies();
			for (vector<CK2Barony*>::iterator baronyItr = baronies.begin(); baronyItr < baronies.end(); baronyItr++)
			{
				learningScore += (*baronyItr)->getTechBonus();
				numBaronies++;
			}
		}
	}

	learningScore /= numBaronies;
}


void EU3Country::determineTechScore()
{
	int numProvinces = 0;
	for (vector<EU3Province*>::iterator provinceItr = provinces.begin(); provinceItr < provinces.end(); provinceItr++)
	{
		vector<CK2Province*> srcProvinces = (*provinceItr)->getSrcProvinces();
		for (vector<CK2Province*>::iterator srcItr = srcProvinces.begin(); srcItr < srcProvinces.end(); srcItr++)
		{
			vector<double> techLevels = (*srcItr)->getTechLevels();
			for (unsigned int i = 0; i < techLevels.size(); i++)
			{
				techScore += techLevels[i] / 24;
			}
			numProvinces++;
		}
	}

	techScore /= numProvinces;
}


void EU3Country::addAcceptedCultures()
{
	map<string, double> cultureTaxes;
	double totalTax = 0.0f;
	for (vector<EU3Province*>::iterator provItr = provinces.begin(); provItr < provinces.end(); provItr++)
	{
		cultureTaxes[ (*provItr)->getCulture() ] += (*provItr)->getBaseTax();
		totalTax += (*provItr)->getBaseTax();
	}
	for (map<string, double>::iterator ctItr = cultureTaxes.begin(); ctItr != cultureTaxes.end(); ctItr++)
	{
		if ((ctItr->second >= 0.05 * totalTax) &&(ctItr->first != primaryCulture))
		{
			acceptedCultures.push_back(ctItr->first);
		}
	}
}


void EU3Country::determineGovernment(double prestigeFactor)
{
	string				srcTitleString		= src->getTitleString();
	CK2Barony*			primaryHolding		= src->getLastHolder()->getPrimaryHolding();
	CK2Title*			srcLiege				= src->getLiege();
	vector<CK2Title*>	srcVassals			= src->getVassals();
	string				highestVassalRank	= "b";
	for (vector<CK2Title*>::iterator vassalItr = srcVassals.begin(); vassalItr < srcVassals.end(); vassalItr++)
	{
		if ((*vassalItr)->getHolder() == src->getHolder())
		{
			continue;
		}
		if ((srcTitleString.substr(0,2) != "k_") && (*vassalItr)->getTitleString().substr(0,2) == "k_")
		{
			highestVassalRank = "k";
		}
		else if ( (srcTitleString.substr(0,2) != "d_") && ((*vassalItr)->getTitleString().substr(0,2) == "d_") && (highestVassalRank != "k") )
		{
			highestVassalRank = "d";
		}
		else if ( (srcTitleString.substr(0,2) != "c_") && ((*vassalItr)->getTitleString().substr(0,2) == "c_") && ((highestVassalRank != "k") || (highestVassalRank != "d")) )
		{
			highestVassalRank = "c";
		}
	}

	if (srcTitleString == "k_papal_state")
	{
		government = "papal_government";
	}
	else if (  ( (srcTitleString == "e_golden_horde") || (srcTitleString == "e_il-khanate") || (srcTitleString == "e_timurids") ) && (src->getLastHolder()->getReligion()->getGroup() != "christian")  )
	{
		government = "steppe_horde";
	}
	else if ( src->getSuccessionLaw() == "patrician_elective")
	{
		government = "merchant_republic";
	}
	else if ( (primaryHolding != NULL) && (primaryHolding->getType() == "city") && (src->isInHRE()) )
	{
		government = "administrative_republic";
		for (vector<EU3Province*>::iterator provItr = provinces.begin(); provItr < provinces.end(); provItr++)
		{
			if ((*provItr)->hasCOT())
			{
				government = "merchant_republic";
			}
		}
	}
	else if ( (primaryHolding != NULL) && (primaryHolding->getType() == "city") )
	{
		government = "noble_republic";
		for (vector<EU3Province*>::iterator provItr = provinces.begin(); provItr < provinces.end(); provItr++)
		{
			if ((*provItr)->hasCOT())
			{
				government = "merchant_republic";
			}
		}
	}
	else if ( (primaryHolding != NULL) && (primaryHolding->getType() == "temple") )
	{
		government = "theocratic_government";
	}
	else if (srcTitleString.substr(0, 2) == "e_")
	{
		government = "imperial_government";
	}
	else if (techGroup == "western")
	{
		government = "feudal_monarchy";
	}
	else if (  (srcLiege != NULL) && ( (srcLiege->getTitleString() == "e_golden_horde") || (srcLiege->getTitleString() == "e_il-khanate") || (srcLiege->getTitleString() == "e_timurids"))  )
	{
		government = "despotic_monarchy";
	}
	else if (srcLiege != NULL)
	{
		government = "feudal_monarchy";
	}
	else if (src->isInHRE())
	{
		government = "feudal_monarchy";
	}
	else if ( (srcTitleString.substr(0,2) == "k_") && (highestVassalRank == "d") )
	{
		government = "feudal_monarchy";
	}
	else if ( (srcTitleString.substr(0,2) == "d_") && (highestVassalRank == "c") )
	{
		government = "feudal_monarchy";
	}
	else if (techGroup == "eastern")
	{
		government = "despotic_monarchy";
	}
	else
	{
		government = "despotic_monarchy";
	}

	CK2Character* holder = src->getHolder();
	if (holder)
	{
		prestige = holder->getTotalScore() / prestigeFactor;
	}
}


void EU3Country::determineEconomy(const cultureGroupMapping& cultureGroups, const map<string, double>& unitPrices)
{
	estimatedIncome = 0.0f;
	for (vector<EU3Province*>::iterator provItr = provinces.begin(); provItr < provinces.end(); provItr++)
	{
		estimatedTax			+= (*provItr)->determineTax(cultureGroups);
		//TODO: Trade
		//TODO: Manus
		estimatedGold			+= (*provItr)->determineGold();
		estimatedProduction	+= (*provItr)->determineProduction(unitPrices);
		//TODO: Vassals
		estimatedTolls			+= (*provItr)->determineTolls();
		//TODO: Harbor fees
	}

	estimatedIncome += estimatedTax;
	//TODO: Trade
	//TODO: Manus
	estimatedIncome += estimatedGold;
	estimatedIncome += estimatedProduction;
	//TODO: Vassals
	estimatedIncome += estimatedTolls;
	//TODO: Harbor fees

	if (monarch != NULL)
	{
		stabilityInvestment = monarch->getAdmin() * 2;
	}
	stabilityInvestment += estimatedIncome / 6;
}



double EU3Country::getTradeEffeciency()
{
	double TE = 0.1f;
	//TODO: tech bonus
	//TODO: slider effects
	if (government == "administrative_republic")
	{
		TE += 0.1;
	}

	return TE;
}

double EU3Country::getProductionEffeciency()
{
	double PE = 0.1f;
	//TODO: tech bonus
	//TODO: slider effects
	if (government == "administrative_republic")
	{
		PE += 0.1;
	}

	return PE;
}


void EU3Country::setPreferredUnitType()
{
	if(techGroup == "western")
	{
		infantry = "western_medieval_infantry";
		cavalry = "western_medieval_knights";
		bigShip = "carrack";
		galley = "galley";
		transport = "cog";
	}
	else if(techGroup == "eastern")
	{
		infantry="eastern_medieval_infantry";
		cavalry="eastern_knights";
		bigShip = "carrack";
		galley = "galley";
		transport = "cog";
	}
	else if(techGroup == "ottoman")
	{
		infantry = "ottoman_yaya";
		cavalry = "ottoman_musellem";
		bigShip = "carrack";
		galley = "galley";
		transport = "cog";
	}
	else if(techGroup == "muslim")
	{
		infantry = "mamluk_archer";
		cavalry = "muslim_cavalry_archers";
		bigShip = "carrack";
		galley = "galley";
		transport = "cog";
	}
	else if(techGroup == "indian")
	{
		infantry = "indian_footsoldier";
		cavalry = "indian_archers";
		bigShip = "carrack";
		galley = "galley";
		transport = "cog";
	}
	else if(techGroup == "chinese")
	{
		infantry = "chinese_longspear";
		cavalry = "eastern_bow";
		bigShip = "carrack";
		galley = "galley";
		transport = "cog";
	}
	else if(techGroup == "sub_saharan")
	{
		infantry = "african_spearmen";
		cavalry = "";
		bigShip = "";
		galley = "";
		transport = "cog";
	}
	else if(techGroup == "new_world")
	{
		infantry = "south_american_spearmen";
		cavalry = "";
		bigShip = "";
		galley = "";
		transport = "";
	}
	else if(techGroup == "nomad_group")
	{
		infantry = "mongol_bow";
		cavalry = "mongol_swarm";
		bigShip = "carrack";
		galley = "galley";
		transport = "cog";
	}
}


void EU3Country::determineTechLevels(const vector<double>& avgTechLevels, const EU3Tech* techData)
{
	vector<double> techLevels;
	for (unsigned int i = 0; i <= TECH_LEGALISM; i++)
	{
		techLevels.push_back(0.0f);
	}
	int numProvinces = 0;
	for (vector<EU3Province*>::iterator itr = provinces.begin(); itr < provinces.end(); itr++)
	{
		vector<CK2Province*> srcProvinces = (*itr)->getSrcProvinces();
		for (vector<CK2Province*>::iterator itr2 = srcProvinces.begin(); itr2 < srcProvinces.end(); itr2++)
		{
			vector<double> provinceTechLevels = (*itr2)->getTechLevels();
			for (unsigned int i = 0; i <= TECH_LEGALISM; i++)
			{
				techLevels[i] += provinceTechLevels[i];
			}
			numProvinces++;
		}
	}
	for (unsigned int i = 0; i <= TECH_LEGALISM; i++)
	{
		if (numProvinces > 0)
		{
			techLevels[i] /= numProvinces;
			techLevels[i] -= avgTechLevels[i];
		}
	}

	double oldLandTech =
		techLevels[TECH_BOWS] +
		techLevels[TECH_LIGHT_ARMOUR] +
		techLevels[TECH_HEAVY_ARMOUR] +
		techLevels[TECH_INFANTRY_MELEE_WEAPONS] +
		techLevels[TECH_CAVALRY_MELEE_WEAPONS] +
		techLevels[TECH_SIEGE_EQUIPMENT] +
		techLevels[TECH_CASTLES] +
		techLevels[TECH_TACTICS];
	double oldNavalTech =
		techLevels[TECH_BOWS] +
		techLevels[TECH_LIGHT_ARMOUR] +
		techLevels[TECH_HEAVY_ARMOUR] +
		techLevels[TECH_INFANTRY_MELEE_WEAPONS] +
		techLevels[TECH_SIEGE_EQUIPMENT] +
		techLevels[TECH_TACTICS];
	double oldTradeTech = 
		techLevels[TECH_CASTLE_TAXES] +
		techLevels[TECH_CITY_TAXES] +
		techLevels[TECH_TEMPLE_TAXES];
	double oldProdTech = 
		techLevels[TECH_CASTLE_CONSTRUCTION] +
		techLevels[TECH_CITY_CONSTRUCTION] +
		techLevels[TECH_TEMPLE_CONSTRUCTION] +
		techLevels[TECH_FORTIFICATIONS_CONSTRUCTION] +
		techLevels[TECH_CONSTRUCTION];
	double oldGovTech = 
		techLevels[TECH_TEMPLE_TAXES] +
		techLevels[TECH_NOBLE_CUSTOMS] +
		techLevels[TECH_POPULAR_CUSTOMS] +
		techLevels[TECH_RELIGIOUS_CUSTOMS] +
		techLevels[TECH_MAJESTY] +
		techLevels[TECH_SPIRITUAL_ART] +
		techLevels[TECH_CULTURE_FLEX] +
		techLevels[TECH_RELIGION_FLEX] +
		techLevels[TECH_LEGALISM];

	governmentTech	= techData->getGovernmentTech("western")	+ (oldGovTech / 9);
	productionTech	= techData->getProductionTech("western")	+ (oldProdTech / 9);
	tradeTech		= techData->getTradeTech("western")			+ (oldTradeTech / 9);
	navalTech		= techData->getNavalTech("western")			+ (oldNavalTech / 9);
	landTech			= techData->getLandTech("western")			+ (oldLandTech / 9);
}


void EU3Country::determineTechInvestment(const EU3Tech* techData, date startDate)
{
	double groupModifier = 1.0f / techData->getGroupModifier(techGroup);

	double sizeModifier;
	int size = provinces.size();
	if (size == 0)
	{
		sizeModifier = 0.2;
	}
	else if (size <= 3)
	{
		sizeModifier = 0.2 * (size - 1);
	}
	else if (size <= 5)
	{
		sizeModifier = 0.1 * (size - 3) + 0.4;
	}
	else if (size == 6)
	{
		sizeModifier = 0.2 * (size - 5) + 0.6;
	}
	else if (size <= 8)
	{
		sizeModifier = 0.1 * (size - 6) + 0.8;
	}
	else
	{
		sizeModifier = 0.025 * (size - 8) + 1.0;
	}

	//Todo: slider effects

	double governmentTechCost	= techData->getGovernmentBaseCost(startDate, (int)governmentTech + 1) * groupModifier * sizeModifier;
	double productionTechCost	= techData->getProductionBaseCost(startDate, (int)productionTech + 1) * groupModifier * sizeModifier;
	double tradeTechCost			= techData->getTradeBaseCost(startDate, (int)tradeTech + 1) * groupModifier * sizeModifier;
	double navalTechCost			= techData->getNavalBaseCost(startDate, (int)navalTech + 1) * groupModifier * sizeModifier;
	double landTechCost			= techData->getLandBaseCost(startDate, (int)landTech + 1) * groupModifier * sizeModifier;

	governmentTechInvestment	= governmentTechCost	* (governmentTech	- (int)governmentTech);
	productionTechInvestment	= productionTechCost	* (productionTech	- (int)productionTech);
	tradeTechInvestment			= tradeTechCost		* (tradeTech		- (int)tradeTech);
	navalTechInvestment			= navalTechCost		* (navalTech		- (int)navalTech);
	landTechInvestment			= landTechCost			* (landTech			- (int)landTech);
}


void EU3Country::determineStartingAgents()
{
	CK2Character* holder = src->getHolder();
	if (!holder)
	{
		merchants = colonists = diplomats = missionaries = spies = magistrates = 0.0;
		return;
	}

	if (src->getTitleString() == "k_scotland")
		holder = holder;

	merchants		= min(holder->getStateStats()[STEWARDSHIP]	/ 15.0, 5.0);
	colonists		= min(holder->getStateStats()[MARTIAL]		/ 15.0, 5.0);
	diplomats		= min(holder->getStateStats()[DIPLOMACY]	/ 15.0, 5.0);
	missionaries	= min(holder->getStateStats()[LEARNING]		/ 15.0, 5.0);
	spies			= min(holder->getStateStats()[INTRIGUE]		/ 15.0, 5.0);
	magistrates		= min(holder->getStateStats()[STEWARDSHIP]	/ 15.0, 5.0);
}


vector<EU3Country*> EU3Country::convertVassals(int initialScore, EU3Diplomacy* diplomacy)
{
	if (vassals.size() == 0)
	{
		vector<EU3Country*> absorbedCountries;
		return absorbedCountries;
	}

	// Am I a junior in a PU?
	if (src->isIndependent() && (src != src->getHolder()->getPrimaryTitle()))
	{
		log("\t%s is completely absorbing all vassals (junior in PU).\n", src->getTitleString().c_str());
		vector<EU3Country*> absorbedCountries = eatVassals();
		return absorbedCountries;
	}

	int score = initialScore;
	string CA = src->getCA();
	if ( CA == "centralization_0")
	{
		score += 1000;
	}
	else if ( CA == "centralization_1")
	{
		score += 2000;
	}
	else if ( CA == "centralization_2")
	{
		score += 3000;
	}
	else if ( CA == "centralization_3")
	{
		score += 4000;
	}
	else if ( CA == "centralization_4")
	{
		score += 5000;
	}

	string srcTitle = this->getSrcCountry()->getTitleString();

	vector<EU3Country*> absorbedCountries;
	vector<EU3Country*> absorbedVassals;
	for(unsigned int i = 0; i < vassals.size(); i++)
	{
		// one's own titles should generally just merge completely
		if	(	(src->getHolder() == vassals[i]->getSrcCountry()->getHolder()) &&
				(src->getHeir() == vassals[i]->getSrcCountry()->getHeir()) &&
				(src->getSuccessionLaw() == vassals[i]->getSrcCountry()->getSuccessionLaw()) &&
				(src->getTitleString() != Configuration::getHRETitle()) &&
				(vassals[i]->getSrcCountry()->getTitleString() != Configuration::getHRETitle())
			)
		{
			//vector<EU3Country*> newlyAbsorbed = vassals[i]->convertVassals(initialScore, diplomacy);
			log("\t%s is completely absorbing %s (same holder).\n", src->getTitleString().c_str(), vassals[i]->getSrcCountry()->getTitleString().c_str());
			for (vector<EU3Province*>::iterator provinceItr = vassals[i]->provinces.begin(); provinceItr != vassals[i]->provinces.end(); provinceItr++)
			{
				provinces.push_back(*provinceItr);
				(*provinceItr)->setOwner(this);
			}
			for (vector<EU3Province*>::iterator coreItr = vassals[i]->cores.begin(); coreItr != vassals[i]->cores.end(); coreItr++)
			{
				cores.push_back(*coreItr);
				(*coreItr)->removeCore(vassals[i]);
				(*coreItr)->addCore(this);
			}
			for (vector<EU3Advisor*>::iterator advisorItr = vassals[i]->advisors.begin(); advisorItr != vassals[i]->advisors.end(); advisorItr++)
			{
				advisors.push_back(*advisorItr);
				(*advisorItr)->setHome(this);
			}
			for (vector<EU3Country*>::iterator subVassalItr = vassals[i]->vassals.begin(); subVassalItr != vassals[i]->vassals.end(); subVassalItr++)
			{
				vassals.push_back(*subVassalItr);
				(*subVassalItr)->liege = this;
			}
			for (vector<EU3Agreement*>::iterator agreementItr = vassals[i]->agreements.begin(); agreementItr != vassals[i]->agreements.end(); agreementItr++)
			{
				diplomacy->removeAgreement(*agreementItr);
			}
			vassals[i]->provinces.clear();
			vassals[i]->cores.clear();
			vassals[i]->advisors.clear();
			vassals[i]->vassals.clear();
			absorbedCountries.push_back(vassals[i]);
			absorbedVassals.push_back(vassals[i]);
			continue;
		}

		// If held by another ruler, a vassal should have no sub-vassals
		vector<EU3Country*> newlyAbsorbed = vassals[i]->eatVassals();
		for (vector<EU3Country*>::iterator newItr = newlyAbsorbed.begin(); newItr != newlyAbsorbed.end(); newItr++)
		{
			absorbedCountries.push_back(*newItr);
		}

		int vassalScore = score;
		CK2Character* liege	= this->getSrcCountry()->getHolder();
		CK2Character* vassal	= vassals[i]->getSrcCountry()->getHolder();
		vassalScore += vassal->getOpinionOf(liege);

		if ((vassalScore >= 5050) && (vassals[i]->getAbsorbScore() < 5050))
		{
			log("\t%s is completely absorbing %s.\n", src->getTitleString().c_str(), vassals[i]->getSrcCountry()->getTitleString().c_str());
			vassals[i]->setAbsorbScore(vassalScore);
			for (vector<EU3Province*>::iterator provinceItr = vassals[i]->provinces.begin(); provinceItr != vassals[i]->provinces.end(); provinceItr++)
			{
				provinces.push_back(*provinceItr);
				(*provinceItr)->setOwner(this);
			}
			for (vector<EU3Province*>::iterator coreItr = vassals[i]->cores.begin(); coreItr != vassals[i]->cores.end(); coreItr++)
			{
				cores.push_back(*coreItr);
				(*coreItr)->removeCore(vassals[i]);
				(*coreItr)->addCore(this);
			}
			for (vector<EU3Advisor*>::iterator advisorItr = vassals[i]->advisors.begin(); advisorItr != vassals[i]->advisors.end(); advisorItr++)
			{
				advisors.push_back(*advisorItr);
				(*advisorItr)->setHome(this);
			}
			for (vector<EU3Country*>::iterator subVassalItr = vassals[i]->vassals.begin(); subVassalItr != vassals[i]->vassals.end(); subVassalItr++)
			{
				vassals.push_back(*subVassalItr);
				(*subVassalItr)->liege = this;
			}
			vassals[i]->provinces.clear();
			vassals[i]->cores.clear();
			vassals[i]->advisors.clear();
			vassals[i]->vassals.clear();
			absorbedCountries.push_back(vassals[i]);
			absorbedVassals.push_back(vassals[i]);
		}
		else if ((vassalScore >= 4000) && (vassals[i]->getAbsorbScore() < 4000))
		{
			log("\t%s is absorbing %s but leaving cores.\n", src->getTitleString().c_str(), vassals[i]->getSrcCountry()->getTitleString().c_str());
			vassals[i]->setAbsorbScore(vassalScore);
			for (vector<EU3Province*>::iterator provinceItr = vassals[i]->provinces.begin(); provinceItr != vassals[i]->provinces.end(); provinceItr++)
			{
				provinces.push_back(*provinceItr);
				(*provinceItr)->setOwner(this);
			}
			for (vector<EU3Advisor*>::iterator advisorItr = vassals[i]->advisors.begin(); advisorItr != vassals[i]->advisors.end(); advisorItr++)
			{
				advisors.push_back(*advisorItr);
				(*advisorItr)->setHome(this);
			}
		}
		else if ((vassalScore >= 2900) && (vassals[i]->getAbsorbScore() < 2900))
		{
			log("\t%s is vassalizing %s and getting cores.\n", src->getTitleString().c_str(), vassals[i]->getSrcCountry()->getTitleString().c_str());
			vassals[i]->setAbsorbScore(vassalScore);
			for (vector<EU3Province*>::iterator provinceItr = vassals[i]->provinces.begin(); provinceItr != vassals[i]->provinces.end(); provinceItr++)
			{
				cores.push_back(*provinceItr);
				(*provinceItr)->addCore(this);
			}
			EU3Agreement* newAgreement = new EU3Agreement;
			newAgreement->type			= "vassal";
			newAgreement->country1	= this;
			newAgreement->country2	= vassals[i];
			newAgreement->startDate	= (date)"1066.9.15";	//TODO: add better starting date
			diplomacy->addAgreement(newAgreement);
			agreements.push_back(newAgreement);
		}
		else if ((vassalScore >= 1900) && (vassals[i]->getAbsorbScore() < 1900))
		{
			log("\t%s is vassalizing %s.\n", src->getTitleString().c_str(), vassals[i]->getSrcCountry()->getTitleString().c_str());
			vassals[i]->setAbsorbScore(vassalScore);
			EU3Agreement* newAgreement = new EU3Agreement;
			newAgreement->type			= "vassal";
			newAgreement->country1	= this;
			newAgreement->country2	= vassals[i];
			newAgreement->startDate	= (date)"1066.9.15";	//TODO: add better starting date
			diplomacy->addAgreement(newAgreement);
		}
		else if ((vassalScore >= 1000) && (vassals[i]->getAbsorbScore() < 1000))
		{
			log("\t%s is sphering and alliancing %s.\n", src->getTitleString().c_str(), vassals[i]->getSrcCountry()->getTitleString().c_str());
			vassals[i]->setAbsorbScore(vassalScore);
			EU3Agreement* newAgreement = new EU3Agreement;
			newAgreement->type			= "sphere";
			newAgreement->country1	= this;
			newAgreement->country2	= vassals[i];
			newAgreement->startDate	= (date)"1066.9.15";	//TODO: add better starting date
			diplomacy->addAgreement(newAgreement);
			agreements.push_back(newAgreement);
			newAgreement = new EU3Agreement;
			newAgreement->type			= "alliance";
			newAgreement->country1	= this;
			newAgreement->country2	= vassals[i];
			newAgreement->startDate	= (date)"1066.9.15";	//TODO: add better starting date
			diplomacy->addAgreement(newAgreement);
			agreements.push_back(newAgreement);
		}
		else if (vassals[i]->getAbsorbScore() <= 0)
		{
			log("\t%s and %s and guaranteeing each other.\n", src->getTitleString().c_str(), vassals[i]->getSrcCountry()->getTitleString().c_str());
			vassals[i]->setAbsorbScore(vassalScore);
			EU3Agreement* newAgreement = new EU3Agreement;
			newAgreement->type			= "guarantee";
			newAgreement->country1	= this;
			newAgreement->country2	= vassals[i];
			newAgreement->startDate	= (date)"1066.9.15";	//TODO: add better starting date
			diplomacy->addAgreement(newAgreement);
			agreements.push_back(newAgreement);
			newAgreement = new EU3Agreement;
			newAgreement->type			= "guarantee";
			newAgreement->country1	= vassals[i];
			newAgreement->country2	= this;
			newAgreement->startDate	= (date)"1066.9.15";	//TODO: add better starting date
			diplomacy->addAgreement(newAgreement);
			agreements.push_back(newAgreement);
		}
	}

	for (vector<EU3Country*>::iterator absorbedItr = absorbedVassals.begin(); absorbedItr != absorbedVassals.end(); absorbedItr++)
	{
		for(vector<EU3Country*>::iterator vassalItr = vassals.begin(); vassalItr != vassals.end(); vassalItr++)
		{
			if (*absorbedItr == *vassalItr)
			{
				vassals.erase(vassalItr);
				break;
			}
		}
	}

	return absorbedCountries;
}


vector<EU3Country*> EU3Country::eatVassals()
{
	vector<EU3Country*> absorbedCountries = vassals;
	for (vector<EU3Country*>::iterator vassalItr = vassals.begin(); vassalItr != vassals.end(); vassalItr++)
	{
		vector<EU3Country*> newAbsorbedCountries = (*vassalItr)->eatVassals();
		for (vector<EU3Country*>::iterator absorbedItr = newAbsorbedCountries.begin(); absorbedItr != newAbsorbedCountries.end();  absorbedItr++)
		{
			absorbedCountries.push_back(*absorbedItr);
		}
		for (vector<EU3Province*>::iterator provinceItr = (*vassalItr)->provinces.begin(); provinceItr != (*vassalItr)->provinces.end(); provinceItr++)
		{
			provinces.push_back(*provinceItr);
			(*provinceItr)->setOwner(this);
		}
		for (vector<EU3Province*>::iterator coreItr = (*vassalItr)->cores.begin(); coreItr != (*vassalItr)->cores.end(); coreItr++)
		{
			cores.push_back(*coreItr);
			(*coreItr)->removeCore(*vassalItr);
			(*coreItr)->addCore(this);
		}
		for (vector<EU3Advisor*>::iterator advisorItr = (*vassalItr)->advisors.begin(); advisorItr != (*vassalItr)->advisors.end(); advisorItr++)
		{
			advisors.push_back(*advisorItr);
			(*advisorItr)->setHome(this);
		}
	}
	vassals.clear();

	return absorbedCountries;
}


void EU3Country::eatVassal(EU3Country* vassal)
{
	vassal->eatVassals();
	for (vector<EU3Province*>::iterator provinceItr = vassal->provinces.begin(); provinceItr != vassal->provinces.end(); provinceItr++)
	{
		provinces.push_back(*provinceItr);
		(*provinceItr)->setOwner(this);
	}
	for (vector<EU3Province*>::iterator coreItr = vassal->cores.begin(); coreItr != vassal->cores.end(); coreItr++)
	{
		cores.push_back(*coreItr);
		(*coreItr)->removeCore(vassal);
		(*coreItr)->addCore(this);
	}
	for (vector<EU3Advisor*>::iterator advisorItr = vassal->advisors.begin(); advisorItr != vassal->advisors.end(); advisorItr++)
	{
		advisors.push_back(*advisorItr);
		(*advisorItr)->setHome(this);
	}
	for (vector<EU3Country*>::iterator vassalItr = vassals.begin(); vassalItr != vassals.end(); vassalItr++)
	{
		if (*vassalItr == vassal)
		{
			vassals.erase(vassalItr);
			break;
		}
	}
}


void EU3Country::replaceWith(EU3Country* convertedCountry, const provinceMapping& provinceMappings)
{
	src								= convertedCountry->src;
	src->setDstCountry(this);
	advisors							= convertedCountry->advisors;
	learningScore					= convertedCountry->learningScore;

	//tag - keep the tag
	//historyFile - keep the history file
	government						= convertedCountry->government;
	primaryCulture					= convertedCountry->primaryCulture;
	acceptedCultures				= convertedCountry->acceptedCultures;
	religion							= convertedCountry->religion;
	monarch							= convertedCountry->monarch;
	heir								= convertedCountry->heir;
	regent							= convertedCountry->regent;
	history							= convertedCountry->history;
	previousMonarchs				= convertedCountry->previousMonarchs;
	techGroup						= convertedCountry->techGroup;
	governmentTech					= convertedCountry->governmentTech;
	productionTech					= convertedCountry->productionTech;
	tradeTech						= convertedCountry->tradeTech;
	navalTech						= convertedCountry->navalTech;
	landTech							= convertedCountry->landTech;
	governmentTechInvestment	= convertedCountry->governmentTech;
	productionTechInvestment	= convertedCountry->productionTech;
	tradeTechInvestment			= convertedCountry->tradeTech;
	navalTechInvestment			= convertedCountry->navalTech;
	landTechInvestment			= convertedCountry->landTech;

	capital							= convertedCountry->capital;
	stability						= convertedCountry->stability;
	stabilityInvestment			= convertedCountry->stabilityInvestment;
	prestige							= convertedCountry->prestige;

	estimatedIncome				= convertedCountry->estimatedIncome;
	estimatedTax					= convertedCountry->estimatedTax;
	estimatedGold					= convertedCountry->estimatedGold;
	estimatedProduction			= convertedCountry->estimatedProduction;
	estimatedTolls					= convertedCountry->estimatedTolls;

	daimyo							= convertedCountry->daimyo;
	japaneseEmperor				= convertedCountry->japaneseEmperor;

	infantry							= convertedCountry->infantry;
	cavalry							= convertedCountry->cavalry;
	bigShip							= convertedCountry->bigShip;
	galley							= convertedCountry->galley;
	transport						= convertedCountry->transport;

	merchants		= convertedCountry->merchants;
	colonists		= convertedCountry->colonists;
	diplomats		= convertedCountry->diplomats;
	missionaries	= convertedCountry->missionaries;
	spies				= convertedCountry->spies;
	magistrates		= convertedCountry->magistrates;

	vector<EU3Province*> newProvinces;
	for (vector<EU3Province*>::iterator provItr = provinces.begin(); provItr != provinces.end(); provItr++)
	{
		provinceMapping::const_iterator provMap = provinceMappings.find( (*provItr)->getNum() );
		if (provMap->second[0] > 0)
		{
			newProvinces.push_back(*provItr);
			(*provItr)->setOwner(this);
		}
	}
	for (vector<EU3Province*>::iterator provItr = convertedCountry->provinces.begin(); provItr != convertedCountry->provinces.end(); provItr++)
	{
		newProvinces.push_back(*provItr);
		(*provItr)->setOwner(this);
	}
	provinces = newProvinces;

	vector<EU3Province*> newCores;
	for (vector<EU3Province*>::iterator provItr = cores.begin(); provItr != cores.end(); provItr++)
	{
		provinceMapping::const_iterator provMap = provinceMappings.find( (*provItr)->getNum() );
		if (provMap->second[0] > 0)
		{
			newCores.push_back(*provItr);
			(*provItr)->addCore(this);
		}
	}
	for (vector<EU3Province*>::iterator provItr = convertedCountry->cores.begin(); provItr != convertedCountry->cores.end(); provItr++)
	{
		newCores.push_back(*provItr);
		(*provItr)->removeCore(convertedCountry);
		(*provItr)->addCore(this);
	}
	cores = newCores;

	for (vector<EU3Advisor*>::iterator advisorItr = convertedCountry->advisors.begin(); advisorItr != convertedCountry->advisors.end(); advisorItr++)
	{
		advisors.push_back(*advisorItr);
		(*advisorItr)->setHome(this);
	}
}


void EU3Country::convertArmiesandNavies(const inverseProvinceMapping inverseProvinceMap, map<int, EU3Province*> allProvinces)
{
	if (src != NULL)
	{
		vector<CK2Army*> srcArmies = src->getHolder()->getArmies();
		for (unsigned int i = 0; i < srcArmies.size(); i++)
		{
			EU3Army* newArmy = new EU3Army(srcArmies[i], inverseProvinceMap, infantry, cavalry, allProvinces, manpower);
			if (newArmy->getNumRegiments() > 0)
			{
				armies.push_back(newArmy);
			}
		}

		double levyMultiplier = 1.0;
		if (src->getTitleString().substr(0,1) == "e")
		{
			levyMultiplier = 1.0 / 2.0;
		}
		else if (src->getTitleString().substr(0,1) == "k")
		{
			levyMultiplier = 2.0 / 3.0;
		}
		else if (src->getTitleString().substr(0,1) == "d")
		{
			levyMultiplier = 3.0 / 4.0;
		}
		else if (src->getTitleString().substr(0,1) == "c")
		{
			levyMultiplier = 4.0 / 5.0;
		}
		int numNavies = 0;
		for (vector<EU3Province*>::iterator itr = provinces.begin(); itr < provinces.end(); itr++)
		{
			int numSrcShips = 0;
			vector<CK2Province*> srcProvinces = (*itr)->getSrcProvinces();
			for (vector<CK2Province*>::iterator itr2 = srcProvinces.begin(); itr2 < srcProvinces.end(); itr2++)
			{
				vector<CK2Barony*> srcBaronies = (*itr2)->getBaronies();
				for (vector<CK2Barony*>::iterator itr3 = srcBaronies.begin(); itr3 != srcBaronies.end(); itr3++)
				{
					manpower		+= levyMultiplier * ((*itr3)->getPSE() / 3000);
					numSrcShips	+=	(*itr3)->getShips();
				}
			}
			if (((*itr)->isCoastal()) && (numSrcShips > 0))
			{
				EU3Navy* newNavy = new EU3Navy(numNavies, 1, (*itr)->getNum(), transport, allProvinces);
				navies.push_back(newNavy);
				numNavies++;
			}
		}

		vector<CK2Army*> srcNavies = src->getHolder()->getNavies();
		for (unsigned int i = 0; i < srcNavies.size(); i++)
		{
			EU3Navy* newNavy = new EU3Navy(srcNavies[i], inverseProvinceMap, transport, infantry, cavalry, allProvinces, manpower);
			if (newNavy->getNumShips() > 0)
			{
				navies.push_back(newNavy);
			}
		}
	}
}