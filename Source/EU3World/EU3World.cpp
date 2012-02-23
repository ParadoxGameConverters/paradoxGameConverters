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
	outputTempHeader(output);
	for (unsigned int i = 0; i < provinces.size(); i++)
	{
		provinces[i].output(output);
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
		_itoa_s(rotwProvinces[i], num, 4, 10);
		
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
		newProvince.init(rotwProvinces[i], obj);

		//add to provinces list
		provinces.push_back(newProvince);
	}
}