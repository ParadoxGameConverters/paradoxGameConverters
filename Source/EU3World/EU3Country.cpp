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
#include "EU3Ruler.h"
#include "EU3History.h"
#include "EU3Province.h"
#include "EU3World.h"
#include <fstream>
using namespace std;


EU3Country::EU3Country(EU3World* world, string newTag, string newHistoryFile, date startDate)
{
	src				= NULL;
	provinces.clear();
	learningScore	= 0.0;

	tag			= newTag;
	historyFile	= newHistoryFile;

	// Parse history file
	Object* obj;
	obj = doParseFile( (Configuration::getEU3Path() + "/history/countries/" + historyFile).c_str() );

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

	vector<Object*> capitalObj = obj->getValue("capital");
	if (capitalObj.size() > 0)
	{
		capital = atoi( capitalObj[0]->getLeaf().c_str() );
	}
	else
	{
		capital = 0;
	}

	stability				= 1.0f;
	stabilityInvestment	= 0.0f;

	vector<Object*> daimyoObj = obj->getValue("daimyo");
	if (daimyoObj.size() > 0)
	{
		string leaf = daimyoObj[0]->getLeaf();
		if (leaf == "emperor")
		{
			daimyo				= true;
			japaneseEmperor	= true;
			world->setJapaneseEmperor(tag);
		}
		else
		{
			daimyo				= true;
			japaneseEmperor	= false;
			world->addDamiyo(tag);
		}
	}
	else
	{
		daimyo				= false;
		japaneseEmperor	= false;
	}

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
					world->setShogun(tag);
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
	if (techGroup != "")
	{
		fprintf(output, "\t\ttechnology_group=%s\n", techGroup.c_str());
	}
	else
	{
		fprintf(output, "\t\ttechnology_group=new_world\n");
	}
	if (primaryCulture != "")
	{
		fprintf(output, "\t\tprimary_culture=%s\n", primaryCulture.c_str());
	}
	for (unsigned int i = 0; i < acceptedCultures.size(); i++)
	{
		fprintf(output, "\t\tadd_accepted_culture=%s\n", acceptedCultures[i].c_str());
	}
	if (religion != "")
	{
		fprintf(output, "\t\treligion=%s\n", religion.c_str());
	}
	if (capital != 0)
	{
		fprintf(output, "\t\tcapital=%d\n", capital);
	}
	for (unsigned int i = 0; i < history.size(); i++)
	{
		history[i]->output(output);
	}
	fprintf(output, "\t}\n");
	if (government != "")
	{
		fprintf(output, "\tgovernment=%s\n", government.c_str());
	}
	else
	{
		fprintf(output, "\tgovernment=tribal_despotism\n");
	}
	if (techGroup != "")
	{
		fprintf(output, "\ttechnology_group=%s\n", techGroup.c_str());
	}
	else
	{
		fprintf(output, "\ttechnology_group=new_world\n");
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
	if (capital != 0)
	{
		fprintf(output, "\tcapital=%d\n", capital);
	}
	fprintf(output, "\tstability=%f\n", stability);
	fprintf(output, "\tstability_investment=%f\n", stabilityInvestment);
	fprintf(output, "\tinflation=0.000\n");
	fprintf(output, "\tlast_bankrupt=\"1.1.1\"\n");
	fprintf(output, "\twartax=\"1.1.1\"\n");
	fprintf(output, "\twar_exhaustion=0.000\n");
	fprintf(output, "\tland_maintenance=1.000\n");
	fprintf(output, "\tnaval_maintenance=1.000\n");
	fprintf(output, "\tcolonial_maintenance=1.000\n");
	fprintf(output, "\tmissionary_maintenance=1.000\n");
	fprintf(output, "\tbadboy=0.000\n");
	fprintf(output, "\tlegitimacy=1.000\n");
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
	

void EU3Country::convert(const CK2Title* _src, const religionMapping& religionMap, const cultureMapping& cultureMap, const inverseProvinceMapping inverseProvinceMap)
{
	src = _src;

	government = "";
	monarch = NULL;
	history.clear();
	previousMonarchs.clear();

	religionMapping::const_iterator religionItr = religionMap.find(src->getHolder()->getReligion());
	if (religionItr != religionMap.end())
	{
		religion = religionItr->second;
	}
	else
	{
		religion = "";
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

	CK2Province* srcCapital = src->getHolder()->getCapital();
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

	stability = 1.0f;

	daimyo				= false;
	japaneseEmperor	= false;
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


void EU3Country::determineGovernment(const religionGroupMapping& religionGroupMap)
{
	string				srcTitleString		= src->getTitleString();
	CK2Barony*			primaryHolding		= src->getHolder()->getPrimaryHolding();
	CK2Title*			liege					= src->getLiege();
	vector<CK2Title*>	vassals				= src->getVassals();
	string				srcReligion			= src->getHolder()->getReligion();
	string				highestVassalRank	= "b";
	for (vector<CK2Title*>::iterator vassalItr = vassals.begin(); vassalItr < vassals.end(); vassalItr++)
	{
		if ((*vassalItr)->getHolder() == src->getHolder())
		{
			continue;
		}
		if ((*vassalItr)->getTitleString().substr(0,2) == "k_")
		{
			highestVassalRank = "k";
		}
		else if ( ((*vassalItr)->getTitleString().substr(0,2) == "d_") && (highestVassalRank != "k") )
		{
			highestVassalRank = "d";
		}
		else if ( ((*vassalItr)->getTitleString().substr(0,2) == "c_") && ((highestVassalRank != "k") || (highestVassalRank != "d")) )
		{
			highestVassalRank = "c";
		}
	}

	if (srcTitleString == "k_papal_state")
	{
		government = "papal_government";
	}
	else if (  ( (srcTitleString == "e_golden_horde") || (srcTitleString == "e_il-khanate") || (srcTitleString == "e_timurids") ) && (religionGroupMap.find(srcReligion)->second != "christian")  )
	{
		government = "steppe_horde";
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
	else if (  (liege != NULL) && ( (liege->getTitleString() == "e_golden_horde") || (liege->getTitleString() == "e_il-khanate") || (liege->getTitleString() == "e_timurids"))  )
	{
		government = "despotic_monarchy";
	}
	else if (liege != NULL)
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
		government = "tribal_democracy";
	}
}


void EU3Country::determineEconomy()
{
	double estimatedIncome = 0.0f;

	if (monarch != NULL)
	{
		stabilityInvestment = monarch->getAdmin() * 2;
	}
	stabilityInvestment += estimatedIncome / 6;
}