#include "EU3World.h"
#include <string>
#include <algorithm>
#include <io.h>
#include "..\Log.h"
#include "..\temp.h"
#include "..\Configuration.h"
#include "..\Parsers\Parser.h"
#include "..\Parsers\Object.h"
#include "..\CK2World\CK2Barony.h"
#include "..\CK2World\CK2Title.h"
#include "..\CK2World\CK2Province.h"
#include	"..\CK2World\CK2World.h"
#include "..\CK2World\CK2Character.h"
#include "EU3Province.h"
#include "EU3Country.h"
#include "EU3Ruler.h"
#include "EU3Advisor.h"
using namespace std;



EU3World::EU3World(CK2World* srcWorld)
{
	startDate = srcWorld->getEndDate();
	provinces.clear();
	countries.clear();
	europeanCountries.clear();
	advisors.clear();
	mapSpreadStrings.clear();

	if (Configuration::getAdvisors() == "normal")
	{
		options[ADVISORS] = 0;
	}
	else if (Configuration::getAdvisors() == "historical")
	{
		options[ADVISORS] = 1;
	}
	else if (Configuration::getAdvisors() == "event")
	{
		options[ADVISORS] = 2;
	}

	if (Configuration::getLeaders() == "normal")
	{
		options[LEADERS] = 0;
	}
	else if (Configuration::getLeaders() == "historical")
	{
		options[LEADERS] = 1;
	}
	else if (Configuration::getLeaders() == "event")
	{
		options[LEADERS] = 2;
	}

	if (Configuration::getColonists() == "normal")
	{
		options[COLONISTS] = 0;
	}
	else if (Configuration::getColonists() == "free")
	{
		options[COLONISTS] = 1;
	}

	if (Configuration::getMerchants() == "normal")
	{
		options[MERCHANTS] = 0;
	}
	else if (Configuration::getMerchants() == "free")
	{
		options[MERCHANTS] = 1;
	}

	if (Configuration::getMissionaries() == "normal")
	{
		options[MISSIONARIES] = 0;
	}
	else if (Configuration::getMissionaries() == "free")
	{
		options[MISSIONARIES] = 1;
	}

	if (Configuration::getInflation() == "normal")
	{
		options[INFLATION] = 0;
	}
	else if (Configuration::getInflation() == "from gold")
	{
		options[INFLATION] = 1;
	}
	else if (Configuration::getInflation() == "none")
	{
		options[INFLATION] = 2;
	}

	if (Configuration::getColonist_size() == "normal")
	{
		options[COLONIST_SIZE] = 0;
	}
	else if (Configuration::getColonist_size() == "200")
	{
		options[COLONIST_SIZE] = 1;
	}
	else if (Configuration::getColonist_size() == "300")
	{
		options[COLONIST_SIZE] = 2;
	}
	else if (Configuration::getColonist_size() == "400")
	{
		options[COLONIST_SIZE] = 3;
	}

	if (Configuration::getDifficulty() == "very easy")
	{
		options[DIFFICULTY] = 0;
	}
	else if (Configuration::getDifficulty() == "easy")
	{
		options[DIFFICULTY] = 1;
	}
	else if (Configuration::getDifficulty() == "normal")
	{
		options[DIFFICULTY] = 2;
	}
	else if (Configuration::getDifficulty() == "hard")
	{
		options[DIFFICULTY] = 3;
	}
	else if (Configuration::getDifficulty() == "very hard")
	{
		options[DIFFICULTY] = 4;
	}

	if (Configuration::getAI_aggressiveness() == "normal")
	{
		options[AI_AGGRESSIVENESS] = 0;
	}
	else if (Configuration::getAI_aggressiveness() == "low")
	{
		options[AI_AGGRESSIVENESS] = 1;
	}
	else if (Configuration::getAI_aggressiveness() == "high")
	{
		options[AI_AGGRESSIVENESS] = 2;
	}

	if (Configuration::getLand_spread() == "25")
	{
		options[LAND_SPREAD] = 0;
	}
	else if (Configuration::getLand_spread() == "normal (50)")
	{
		options[LAND_SPREAD] = 1;
	}
	else if (Configuration::getLand_spread() == "75")
	{
		options[LAND_SPREAD] = 2;
	}
	else if (Configuration::getLand_spread() == "100")
	{
		options[LAND_SPREAD] = 3;
	}
	else if (Configuration::getLand_spread() == "200")
	{
		options[LAND_SPREAD] = 4;
	}

	if (Configuration::getSea_spread() == "25")
	{
		options[SEA_SPREAD] = 0;
	}
	else if (Configuration::getSea_spread() == "normal (50)")
	{
		options[SEA_SPREAD] = 1;
	}
	else if (Configuration::getSea_spread() == "75")
	{
		options[SEA_SPREAD] = 2;
	}
	else if (Configuration::getSea_spread() == "100")
	{
		options[SEA_SPREAD] = 3;
	}
	else if (Configuration::getSea_spread() == "200")
	{
		options[SEA_SPREAD] = 4;
	}

	if (Configuration::getSpies() == "normal")
	{
		options[SPIES] = 0;
	}
	else if (Configuration::getSpies() == "free")
	{
		options[SPIES] = 1;
	}

	if (Configuration::getLucky_nations() == "historical")
	{
		options[LUCKY_NATIONS] = 0;
	}
	else if (Configuration::getLucky_nations() == "random")
	{
		options[LUCKY_NATIONS] = 1;
	}
	else if (Configuration::getLucky_nations() == "none")
	{
		options[LUCKY_NATIONS] = 2;
	}

	japaneseEmperor	= "";
	daimyos.clear();
	shogun				= "";
	shogunPower			= -1.0f;
}


void EU3World::output(FILE* output)
{
	fprintf(output, "date=\"%s\"\n", startDate.toString().c_str());
	outputTempHeader(output);
	fprintf(output, "gameplaysettings=\n");
	fprintf(output, "{\n");
	fprintf(output, "\tsetgameplayoptions=\n");
	fprintf(output, "\t{\n");
	fprintf(output, "\t\t");
	for (unsigned int i = 0; i < OPTIONS_END; i++)
	{
		fprintf(output, "%d ", options[i]);
	}
	fprintf(output, "\n");
	fprintf(output, "\t}\n");
	fprintf(output, "}\n");
	outputTempHeader2(output);
	for (map<int, EU3Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		if (i->second != NULL)
		{
			i->second->output(output);
		}
		else
		{
			log("\tError: EU3 province %d is unmapped!\n", i->first);
		}
	}
	for (map<string, EU3Country*>::iterator i = countries.begin(); i != countries.end(); i++)
	{
		i->second->output(output);
	}
	fprintf(output, "active_advisors=\n");
	fprintf(output, "{\n");
	fprintf(output, "\tnotechgroup=\n");
	fprintf(output, "\t{\n");
	for (unsigned int i = 0; i < advisors.size(); i++)
	{
		advisors[i]->outputInActive(output);
	}
	fprintf(output, "\t}\n");
	fprintf(output, "}\n");
	if (shogunPower != -1.0f)
	{
		fprintf(output, "shogun=\n");
		fprintf(output, "{\n");
		fprintf(output, "\tshogun=%f\n", shogunPower);
		fprintf(output, "\temperor=\n");
		fprintf(output, "\t{\n");
		fprintf(output, "\t\tstatus=0\n");
		fprintf(output, "\t\temperor=1\n");
		fprintf(output, "\t\tback_kampaku=0\n");
		fprintf(output, "\t\tactive=1\n");
		fprintf(output, "\t\tcountry=\"%s\"\n", japaneseEmperor.c_str());
		fprintf(output, "\t}\n");
		for(unsigned int i = 0; i < daimyos.size(); i++)
		{
			fprintf(output, "\tdaimyo=\n");
			fprintf(output, "\t{\n");
			if (daimyos[i] == shogun)
			{
				fprintf(output, "\t\tstatus=3\n");
			}
			else
			{
				fprintf(output, "\t\tstatus=0\n");
			}
			fprintf(output, "\t\temperor=0\n");
			fprintf(output, "\t\tback_kampaku=0\n");
			fprintf(output, "\t\tactive=1\n");
			fprintf(output, "\t\tcountry=\"%s\"\n", daimyos[i].c_str());
			fprintf(output, "\t}\n");
		}
		fprintf(output, "}\n");
	}
}


void EU3World::addPotentialCountries()
{
	string EU3Loc = Configuration::getEU3Path();

	struct _finddata_t	countryDirData;
	intptr_t					fileListing;
	if ( (fileListing = _findfirst( (EU3Loc + "\\history\\countries\\*").c_str(), &countryDirData)) == -1L)
	{
		log("Error: Could not open country history directory.\n");
		return;
	}
	do
	{
		if (strcmp(countryDirData.name, ".") == 0 || strcmp(countryDirData.name, "..") == 0 )
		{
				continue;
		}

		string filename;
		filename = countryDirData.name;

		string tag;
		tag = filename.substr(0, 3);
		transform(tag.begin(), tag.end(), tag.begin(), toupper);

		if (tag == "REB")
		{
			continue;
		}

		EU3Country* newCountry = new EU3Country(this, tag, filename, startDate);
		countries.insert(make_pair(tag, newCountry));

	} while(_findnext(fileListing, &countryDirData) == 0);
	_findclose(fileListing);
}


void EU3World::setupProvinces(provinceMapping& provinceMap)
{
	for (provinceMapping::iterator i = provinceMap.begin(); i != provinceMap.end(); i++)
	{
		//parse relevant file
		Object* obj;
		char num[5];
		_itoa_s(i->first, num, 5, 10);
		
		string filename = Configuration::getEU3Path() + "\\history\\provinces\\" + num + "*-*.txt";
		struct _finddata_t	fileData;
		intptr_t					fileListing;
		if ( (fileListing = _findfirst(filename.c_str(), &fileData)) != -1L)
		{
			obj = doParseFile( (Configuration::getEU3Path() + "\\history\\provinces\\" + fileData.name).c_str() );
			_findclose(fileListing);
		}
		else
		{
			obj = new Object("NULL Object");
		}

		//initialize province
		EU3Province* newProvince = new EU3Province(i->first, obj, startDate, mapSpreadStrings);

		//add to provinces list
		provinces.insert( make_pair(i->first, newProvince) );
	}
}


void EU3World::convertCountries(countryMapping& countryMap, const religionMapping& religionMap, const cultureMapping& cultureMap, const inverseProvinceMapping inverseProvinceMap)
{
	for (countryMapping::iterator i = countryMap.begin(); i != countryMap.end(); i++)
	{
		i->second->convert(i->first, religionMap, cultureMap, inverseProvinceMap);
		europeanCountries.push_back(i->second->getTag());
	}

	vector<string> nomadTech;
	vector<string> westernTech;
	vector<string> easternTech;
	vector<string> ottomanTech;
	vector<string> muslimTech;
	vector<string> indianTech;
	vector<string> chineseTech;
	vector<string> subSaharanTech;
	vector<string> newWorldTech;
	for (map<string, EU3Country*>::iterator i = countries.begin(); i != countries.end(); i++)
	{
		string techGroup = i->second->getTechGroup();
		if (techGroup == "nomad_group")
		{
			nomadTech.push_back( i->first );
		}
		else if (techGroup == "western")
		{
			westernTech.push_back( i->first );
		}
		else if (techGroup == "eastern")
		{
			easternTech.push_back( i->first );
		}
		else if (techGroup == "ottoman")
		{
			ottomanTech.push_back( i->first );
		}
		else if (techGroup == "muslim")
		{
			muslimTech.push_back( i->first );
		}
		else if (techGroup == "indian")
		{
			indianTech.push_back( i->first );
		}
		else if (techGroup == "chinese")
		{
			chineseTech.push_back( i->first );
		}
		else if (techGroup == "sub_saharan")
		{
			subSaharanTech.push_back( i->first );
		}
		else if (techGroup == "new_world")
		{
			newWorldTech.push_back( i->first );
		}

		vector<string> selfString;
		selfString.push_back( i->first );
		mapSpreadStrings.insert(  make_pair( i->first, selfString )  );
	}

	mapSpreadStrings.insert( make_pair("nomad_group", nomadTech) );
	mapSpreadStrings.insert( make_pair("western", westernTech) );
	mapSpreadStrings.insert( make_pair("eastern", easternTech) );
	mapSpreadStrings.insert( make_pair("ottoman", ottomanTech) );
	mapSpreadStrings.insert( make_pair("muslim", muslimTech) );
	mapSpreadStrings.insert( make_pair("indian", indianTech) );
	mapSpreadStrings.insert( make_pair("chinese", chineseTech) );
	mapSpreadStrings.insert( make_pair("sub_saharan", subSaharanTech) );
	mapSpreadStrings.insert( make_pair("new_world", newWorldTech) );
}


void EU3World::convertProvinces(provinceMapping& provinceMap, map<int, CK2Province*>& allSrcProvinces, countryMapping& countryMap, cultureMapping& cultureMap, religionMapping& religionMap, continentMapping& continentMap)
{
	double totalHistoricalBaseTax		= 0.0f;
	double totalHistoricalPopulation = 0.0f;
	double totalHistoricalManpower	= 0.0f;
	for (provinceMapping::const_iterator provItr = provinceMap.begin(); provItr != provinceMap.end(); provItr++)
	{
		if (provItr->second[0] != 0)
		{
			totalHistoricalBaseTax		+= provinces.find(provItr->first)->second->getBaseTax();
			totalHistoricalPopulation	+= provinces.find(provItr->first)->second->getPopulation();
			totalHistoricalManpower		+= provinces.find(provItr->first)->second->getManpower();
		}
	}
	log("\tTotal historical base tax is %f.\n", totalHistoricalBaseTax);
	log("\tTotal historical population is %f.\n", totalHistoricalPopulation);
	log("\tTotal historical manpower is %f.\n", totalHistoricalManpower);

	double totalBaseTaxProxy	= 0.0f;
	double totalPopProxy			= 0.0f;
	double totalManpowerProxy	= 0.0f;
	for (map<int, CK2Province*>::const_iterator srcItr = allSrcProvinces.begin(); srcItr != allSrcProvinces.end(); srcItr++)
	{
		vector<CK2Barony*> baronies = srcItr->second->getBaronies();
		for (vector<CK2Barony*>::const_iterator baronyItr = baronies.begin(); baronyItr != baronies.end(); baronyItr++)
		{
			totalBaseTaxProxy		+= (*baronyItr)->getBaseTaxProxy();
			totalPopProxy			+= (*baronyItr)->getPopProxy();
			totalManpowerProxy	+= (*baronyItr)->getManpowerProxy();
		}
	}

	for(provinceMapping::iterator i = provinceMap.begin(); i != provinceMap.end(); i++)
	{
		if (i->second[0] == 0)
		{
			continue;
		}
		
		vector<int>	srcProvinceNums = i->second;
		vector<CK2Province*> srcProvinces;
		for (unsigned j = 0; j < srcProvinceNums.size(); j++)
		{
			CK2Province* srcProvince = allSrcProvinces[ srcProvinceNums[j] ];
			if (srcProvince != NULL)
			{
				srcProvinces.push_back(srcProvince);
			}
		}

		vector<CK2Barony*> baronies;
		int numBaronies = 0;
		for (unsigned int j = 0; j < srcProvinces.size(); j++)
		{
			vector<CK2Barony*> srcBaronies = srcProvinces[j]->getBaronies();
			for(unsigned int k = 0; k < srcBaronies.size(); k++)
			{
				baronies.push_back(srcBaronies[k]);
				numBaronies++;
			}
		}

		double	baseTaxProxy	= 0.0f;
		double	popProxy			= 0.0f;
		double	manpowerProxy	= 0.0f;
		bool		inHRE		= false;
		vector< pair<const CK2Title*, int > > owners;	// ownerTitle, numBaronies
		for (unsigned int j = 0; j < baronies.size(); j++)
		{
			baseTaxProxy	+= baronies[j]->getBaseTaxProxy();
			popProxy			+= baronies[j]->getPopProxy();
			manpowerProxy	+= baronies[j]->getManpowerProxy();

			const CK2Title* title = baronies[j]->getTitle();
			while( !title->isIndependent() )
			{
				title = title->getLiege();
			}

			bool ownerFound = false;
			for(unsigned int k = 0; k < owners.size(); k++)
			{
				if (owners[k].first == title)
				{
					owners[k].second++;
					ownerFound = true;
				}
			}
			if (!ownerFound)
			{
				owners.push_back( make_pair(title, 1) );
				if (title->isInHRE())
				{
					inHRE = true;
				}
			}
		}

		if (Configuration::getMultipleProvsMethod() == "average")
		{
			baseTaxProxy	/= srcProvinces.size();
			popProxy			/= srcProvinces.size();
			manpowerProxy	/= srcProvinces.size();
		}

		map<int, EU3Province*>::iterator provItr = provinces.find(i->first);
		provItr->second->convert(i->first, inHRE, europeanCountries, srcProvinces);

		const CK2Title*	greatestOwner;
		int					greatestOwnerNum = 0;
		for (unsigned int j = 0; j < owners.size(); j++)
		{
			provItr->second->addCore( countryMap[owners[j].first]->getTag() );
			if (owners[j].second > greatestOwnerNum)
			{
				greatestOwner		= owners[j].first;
				greatestOwnerNum	= owners[j].second;
			}
		}
		if (owners.size() > 0)
		{
			string ownerTag = countryMap[greatestOwner]->getTag();
			provItr->second->setOwner( ownerTag );
			provItr->second->setSrcOwner(greatestOwner);
			countries[ownerTag]->addProvince(provItr->second);
			if ( continentMap[countries[ownerTag]->getCapital()] == continentMap[provItr->first])
			{
				provItr->second->setSameContinent(true);
			}
			else
			{
				provItr->second->setSameContinent(false);
			}
		}

		if (Configuration::getBasetax() == "converted")
		{
			provItr->second->setBaseTax(totalHistoricalBaseTax * baseTaxProxy / totalBaseTaxProxy);
		}
		else if (Configuration::getBasetax() == "blended")
		{
			double blendAmount = atof( Configuration::getBasetaxblendamount().c_str() );
			provItr->second->setBaseTax( (blendAmount * provItr->second->getBaseTax()) + ((1 - blendAmount) * totalHistoricalBaseTax * baseTaxProxy / totalBaseTaxProxy) );
		}
		if (Configuration::getPopulation() == "converted")
		{
			provItr->second->setPopulation(totalHistoricalPopulation * popProxy / totalPopProxy);
		}
		else if (Configuration::getPopulation() == "blended")
		{
			double blendAmount = atof( Configuration::getPopulationblendamount().c_str() );
			provItr->second->setPopulation( (blendAmount * provItr->second->getPopulation()) + ((1 - blendAmount) * totalHistoricalPopulation * popProxy / totalPopProxy) );
		}
		if (Configuration::getManpower() == "converted")
		{
			provItr->second->setManpower(totalHistoricalManpower * manpowerProxy / totalManpowerProxy);
		}
		if (Configuration::getManpower() == "blended")
		{
			double blendAmount = atof( Configuration::getManpowerblendamount().c_str() );
			provItr->second->setManpower( (blendAmount * provItr->second->getManpower()) + ((1 - blendAmount) * totalHistoricalManpower * manpowerProxy / totalManpowerProxy) );
		}
		provItr->second->determineCulture(cultureMap, srcProvinces, baronies);
		provItr->second->determineReligion(religionMap, srcProvinces);
	}
}


void EU3World::addAcceptedCultures()
{
	for(map<string, EU3Country*>::iterator countryItr = countries.begin(); countryItr != countries.end(); countryItr++)
	{
		countryItr->second->addAcceptedCultures();
	}
}


void EU3World::convertAdvisors(inverseProvinceMapping& inverseProvinceMap, provinceMapping& provinceMap, CK2World& srcWorld)
{
	// CK2 Advisors
	map<string, CK2Title*> titles = srcWorld.getAllTitles();
	map<int, CK2Character*> rulers;
	for (map<string, CK2Title*>::iterator i = titles.begin(); i != titles.end(); i++)
	{
		CK2Character* ruler = i->second->getHolder();
		if (ruler != NULL)
		{
			rulers.insert( make_pair(ruler->getNum(), ruler) );
		}
	}

	for (map<int, CK2Character*>::iterator i = rulers.begin(); i != rulers.end(); i++)
	{
		CK2Character** srcAdvisors = i->second->getAdvisors();
		for (unsigned int i = 0; i < 5; i++)
		{
			if (srcAdvisors[i] != NULL)
			{
				EU3Advisor*	newAdvisor		= new EU3Advisor(srcAdvisors[i], inverseProvinceMap, provinces, startDate);
				int			CK2Location		= srcAdvisors[i]->getLocationNum();

				vector<int>		provinceNums;
				provinceNums.clear();
				if (CK2Location != NULL)
				{
					provinceNums = inverseProvinceMap[CK2Location];
				}

				EU3Province*	advisorHome = NULL;
				if (provinceNums.size() > 0)
				{
					advisorHome = provinces[ provinceNums[0] ];
					newAdvisor->setLocation( provinceNums[0] );
				}
				if (advisorHome != NULL)
				{
					advisorHome->addAdvisor(newAdvisor);
					advisors.push_back(newAdvisor);
				}
			}
		}
	}

	// ROTW Advisors
	string EU3Loc = Configuration::getEU3Path();

	struct _finddata_t	advisorDirData;
	intptr_t					fileListing;
	if ( (fileListing = _findfirst( (EU3Loc + "\\history\\advisors\\*").c_str(), &advisorDirData)) == -1L)
	{
		log("\tError: Could not open advisors history directory.\n");
		return;
	}
	do
	{
		if (strcmp(advisorDirData.name, ".") == 0 || strcmp(advisorDirData.name, "..") == 0 )
		{
				continue;
		}

		string filename;
		filename = advisorDirData.name;

		Object* obj;
		obj = doParseFile((Configuration::getEU3Path() + "/history/advisors/" + filename).c_str());

		vector<Object*> advisorsObj = obj->getLeaves();
		for (unsigned int i = 0; i < advisorsObj.size(); i++)
		{
			EU3Advisor* newAdvisor = new EU3Advisor(advisorsObj[i], provinces);
			if ( (newAdvisor->getStartDate() < startDate) && (startDate < newAdvisor->getDeathDate()) )
			{
				vector<int> srcLocationNums = provinceMap[newAdvisor->getLocation()];
				if ( (srcLocationNums.size() == 1) && (srcLocationNums[0] == 0) )
				{
					provinces[newAdvisor->getLocation()]->addAdvisor(newAdvisor);
					advisors.push_back(newAdvisor);
				}
			}
		}

	} while(_findnext(fileListing, &advisorDirData) == 0);
	_findclose(fileListing);
}


void EU3World::convertTech(countryMapping& countryMap, const religionGroupMapping& religionGroupMap)
{
	double highestLearningScore = 0.0f;
	for (countryMapping::iterator countryItr = countryMap.begin(); countryItr != countryMap.end(); countryItr++)
	{
		countryItr->second->determineLearningScore();
		if (countryItr->second->getLearningScore() > highestLearningScore)
		{
			highestLearningScore = countryItr->second->getLearningScore();
		}
	}

	for (countryMapping::iterator countryItr = countryMap.begin(); countryItr != countryMap.end(); countryItr++)
	{
		string religion	= countryItr->first->getHolder()->getReligion();
		string title		= countryItr->first->getTitleString();
		if (  ( (title == "e_golden_horde") || (title == "e_il-khanate") || (title == "e_timurids") ) && (religionGroupMap.find(religion)->second != "christian")  )
		{
			countryItr->second->setTechGroup("nomad_group");
			log("\t%s is in tech group nomad.\n", countryItr->second->getTag().c_str());
		}
		else if (countryItr->second->getLearningScore() <= 0.2 * highestLearningScore)
		{
			countryItr->second->setTechGroup("sub_saharan");
			log("\t%s is in tech group sub saharan.\n", countryItr->second->getTag().c_str());
		}
		else if (countryItr->second->getLearningScore() <= 0.75 * highestLearningScore)
		{
			countryItr->second->setTechGroup("muslim");
			log("\t%s is in tech group muslim.\n", countryItr->second->getTag().c_str());
		}
		else if (countryItr->second->getLearningScore() <= 0.8 * highestLearningScore)
		{
			countryItr->second->setTechGroup("ottoman");
			log("\t%s is in tech group ottoman.\n", countryItr->second->getTag().c_str());
		}
		else if (countryItr->second->getLearningScore() <= 0.85 * highestLearningScore)
		{
			countryItr->second->setTechGroup("eastern");
			log("\t%s is in tech group eastern.\n", countryItr->second->getTag().c_str());
		}
		else
		{
			countryItr->second->setTechGroup("western");
			log("\t%s is in tech group western.\n", countryItr->second->getTag().c_str());
		}
	}
}


void EU3World::convertGovernments(const religionGroupMapping& religionGroupMap)
{
	for (vector<string>::iterator itr = europeanCountries.begin(); itr < europeanCountries.end(); itr++)
	{
		map<string, EU3Country*>::iterator countryItr = countries.find(*itr);
		if (countryItr != countries.end())
		{
			countryItr->second->determineGovernment(religionGroupMap);
		}
	}
}


void EU3World::convertEconomies(const cultureGroupMapping& cultureGroups)
{
	for (map<string, EU3Country*>::iterator countryItr = countries.begin(); countryItr != countries.end(); countryItr++)
	{
		if (countryItr != countries.end())
		{
			countryItr->second->determineEconomy(cultureGroups);
		}
	}
}