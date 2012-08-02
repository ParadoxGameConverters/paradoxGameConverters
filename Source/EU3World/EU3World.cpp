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



EU3World::EU3World()
{
	startDate = (date)"1.1.1";
	provinces.clear();
	countries.clear();
	europeanCountries.clear();
	advisors.clear();
}


void EU3World::output(FILE* output)
{
	fprintf(output, "date=\"%s\"\n", startDate.toString().c_str());
	outputTempHeader(output);
	for (map<int, EU3Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		if (i->second != NULL)
		{
			i->second->output(output);
		}
		else
		{
			log("	Error: EU3 province %d is unmapped!\n", i->first);
		}
	}
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		countries[i]->output(output);
	}
	fprintf(output, "active_advisors=\n");
	fprintf(output, "{\n");
	fprintf(output, "	notechgroup=\n");
	fprintf(output, "	{\n");
	for (unsigned int i = 0; i < advisors.size(); i++)
	{
		advisors[i]->outputInActive(output);
	}
	fprintf(output, "	}\n");
	fprintf(output, "}\n");
}


void EU3World::init(CK2World* srcWorld)
{
	startDate = srcWorld->getEndDate();
}


void EU3World::convertCountries(countryMapping& countryMap)
{
	for (countryMapping::iterator i = countryMap.begin(); i != countryMap.end(); i++)
	{
		i->second->convert(i->first);
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
	for (vector<EU3Country*>::iterator i = countries.begin(); i != countries.end(); i++)
	{
		string techGroup = (*i)->getTechGroup();
		if (techGroup == "nomad_group")
		{
			nomadTech.push_back( (*i)->getTag() );
		}
		else if (techGroup == "western")
		{
			westernTech.push_back( (*i)->getTag() );
		}
		else if (techGroup == "eastern")
		{
			easternTech.push_back( (*i)->getTag() );
		}
		else if (techGroup == "ottoman")
		{
			ottomanTech.push_back( (*i)->getTag() );
		}
		else if (techGroup == "muslim")
		{
			muslimTech.push_back( (*i)->getTag() );
		}
		else if (techGroup == "indian")
		{
			indianTech.push_back( (*i)->getTag() );
		}
		else if (techGroup == "chinese")
		{
			chineseTech.push_back( (*i)->getTag() );
		}
		else if (techGroup == "sub_saharan")
		{
			subSaharanTech.push_back( (*i)->getTag() );
		}
		else if (techGroup == "new_world")
		{
			newWorldTech.push_back( (*i)->getTag() );
		}

		vector<string> selfString;
		selfString.push_back( (*i)->getTag() );
		mapSpreadStrings.insert(  make_pair( (*i)->getTag(), selfString )  );
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

#pragma optimize("", off)
void EU3World::convertProvinces(provinceMapping& provinceMap, map<int, CK2Province*>& allSrcProvinces, countryMapping& countryMap, cultureMapping& cultureMap)
{
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

		bool inHRE = false;
		vector< pair<const CK2Title*, int > > owners;	// ownerTitle, numBaronies
		for (unsigned int j = 0; j < baronies.size(); j++)
		{
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

		EU3Province* newProvince = new EU3Province;
		newProvince->setNumber(i->first);

		const CK2Title*	greatestOwner;
		int					greatestOwnerNum = 0;
		for (unsigned int j = 0; j < owners.size(); j++)
		{
			newProvince->addCore( countryMap[owners[j].first]->getTag() );
			if (owners[j].second > greatestOwnerNum)
			{
				greatestOwner		= owners[j].first;
				greatestOwnerNum	= owners[j].second;
			}
		}
		if (owners.size() > 0)
		{
			newProvince->setOwner( countryMap[greatestOwner]->getTag() );
		}
		newProvince->setInHRE(inHRE);
		newProvince->setDiscoveredBy(europeanCountries);

		newProvince->determineCulture(cultureMap, srcProvinces, baronies);

		provinces.insert( make_pair(i->first, newProvince) );
	}
}
#pragma optimize("", on)

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
		log("	Error: Could not open advisors history directory.\n");
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
			if ( (newAdvisor->getDate() < startDate) && (startDate < newAdvisor->getDeathDate()) )
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


void EU3World::setupRotwProvinces(inverseProvinceMapping& inverseProvinceMap)
{
	vector<int> rotwProvinces = inverseProvinceMap[0];
	for (unsigned int i = 0; i < rotwProvinces.size(); i++)
	{
		//parse relevant file
		Object* obj;
		char num[5];
		_itoa_s(rotwProvinces[i], num, 5, 10);
		
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
		EU3Province* newProvince = new EU3Province;;
		newProvince->init(rotwProvinces[i], obj, startDate, mapSpreadStrings);

		//add to provinces list
		provinces.insert( make_pair(rotwProvinces[i], newProvince) );
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

		EU3Country* newCountry = new EU3Country;
		newCountry->init(tag, filename, startDate);
		countries.push_back(newCountry);

	} while(_findnext(fileListing, &countryDirData) == 0);
	_findclose(fileListing);
}


vector<EU3Country*>	EU3World::getCountries()
{
	return countries;
}