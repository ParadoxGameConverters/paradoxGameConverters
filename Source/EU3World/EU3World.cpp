#include "EU3World.h"
#include <string>
#include <io.h>
#include "..\Log.h"
#include "..\temp.h"
#include "..\Configuration.h"
#include "..\Parsers\Parser.h"
using namespace std;



void EU3World::output(FILE* output)
{
	fprintf(output, "date=\"%s\"\n", startDate.toString().c_str());
	outputTempHeader(output);
	for (unsigned int i = 0; i < provinces.size(); i++)
	{
		provinces[i].output(output);
	}
}


void EU3World::init(CK2World srcWorld)
{
	startDate = srcWorld.getEndDate();
}


void EU3World::convertProvinces(provinceMapping provinceMap, map<int, CK2Province*> allSrcProvinces, countryMapping countryMap)
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
			if (srcProvince == 0)
			{
				log("Could not get information for CK2 province #%d. Sea Province?\n", srcProvinceNums[j]);
			}
			else
			{
				srcProvinces.push_back(srcProvince);
			}
		}

		vector<CK2Barony*> baronies;
		for (unsigned int j = 0; j < srcProvinces.size(); j++)
		{
			vector<CK2Barony*> srcBaronies = srcProvinces[j]->getBaronies();
			for(unsigned int k = 0; k < srcBaronies.size(); k++)
			{
				baronies.push_back(srcBaronies[k]);
			}
		}

		vector< pair<string, int > > owners;	// ownerTitle, numBaronies
		for (unsigned int j = 0; j < baronies.size(); j++)
		{
			CK2Title* title = baronies[j]->getTitle();
			while( !title->isIndependent() )
			{
				title = title->getLiege();
			}

			bool ownerFound = false;
			for(unsigned int k = 0; k < owners.size(); k++)
			{
				if (owners[k].first == title->getTitleString())
				{
					owners[k].second++;
					ownerFound = true;
				}
			}
			if (!ownerFound)
			{
				owners.push_back( make_pair(title->getTitleString(), 1) );
			}
		}

		EU3Province newProvince;
		newProvince.setNumber(i->first);

		string	greatestOwner;
		int		greatestOwnerNum = 0;
		for (unsigned int j = 0; j < owners.size(); j++)
		{
			newProvince.addCore( countryMap[owners[j].first] );
			if (owners[j].second > greatestOwnerNum)
			{
				greatestOwner		= owners[j].first;
				greatestOwnerNum	= owners[j].second;
			}
		}
		newProvince.setOwner( countryMap[greatestOwner] );

		provinces.push_back(newProvince);
	}
}


void EU3World::setupRotwProvinces(inverseProvinceMapping inverseProvinceMap)
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
			log("Error: Could not find EU3 history file for province %d!\n", num);
			continue;
		}

		//initialize province
		EU3Province newProvince;
		newProvince.init(rotwProvinces[i], obj, startDate);

		//add to provinces list
		provinces.push_back(newProvince);
	}
}


void EU3World::addPotentialCountries(ifstream &countriesMapping, string EU3Loc)
{
	while (!countriesMapping.eof())
	{
		string line;
		getline(countriesMapping, line);

		if ( (line.size() < 3) || (line[0] == '#') )
		{
			continue;
		}
		
		string tag;
		tag = line.substr(0, 3);

		string countryFileName;
		int start			= line.find_first_of('/');
		int size				= line.find_last_of('\"') - start;
		countryFileName	= line.substr(start, size);

		if (tag == "REB")
		{
			continue;
		}
		EU3Country newCountry;
		newCountry.init(tag, countryFileName);
		potentialCountries.push_back(newCountry);
	}
}


vector<string>	EU3World::getPotentialTags()
{
	vector<string> tagList;
	for (unsigned int i = 0; i < potentialCountries.size(); i++)
	{
		tagList.push_back(potentialCountries[i].getTag());
	}
	return tagList;
}