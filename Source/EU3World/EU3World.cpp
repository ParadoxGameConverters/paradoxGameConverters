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
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		countries[i]->output(output);
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

		bool inHRE = false;
		vector< pair<CK2Title*, int > > owners;	// ownerTitle, numBaronies
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

		EU3Province newProvince;
		newProvince.setNumber(i->first);

		CK2Title*	greatestOwner;
		int			greatestOwnerNum = 0;
		for (unsigned int j = 0; j < owners.size(); j++)
		{
			newProvince.addCore( countryMap[owners[j].first]->getTag() );
			if (owners[j].second > greatestOwnerNum)
			{
				greatestOwner		= owners[j].first;
				greatestOwnerNum	= owners[j].second;
			}
		}
		if (owners.size() > 0)
		{
			newProvince.setOwner( countryMap[greatestOwner]->getTag() );
		}
		newProvince.setInHRE(inHRE);
		newProvince.setDiscoveredBy(europeanCountries);

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

		if (tag == "REB")
		{
			continue;
		}

		EU3Country* newCountry = new EU3Country;
		newCountry->init(tag, filename);
		countries.push_back(newCountry);

		europeanCountries.push_back(tag);	//TODO: determine this more properly

	} while(_findnext(fileListing, &countryDirData) == 0);
	_findclose(fileListing);
}


vector<EU3Country*>	EU3World::getCountries()
{
	return countries;
}