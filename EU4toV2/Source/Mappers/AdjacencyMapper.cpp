/*Copyright (c) 2017 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#include "AdjacencyMapper.h"
#include "../Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include <fstream>



adjacencyMapper* adjacencyMapper::instance = NULL;



adjacencyMapper::adjacencyMapper()
{
	LOG(LogLevel::Info) << "Importing province adjacencies";
	string filename = getAdjacencyFilename();

	FILE* adjacenciesFile = NULL;
	fopen_s(&adjacenciesFile, filename.c_str(), "rb");
	if (adjacenciesFile == NULL)
	{
		LOG(LogLevel::Error) << "Could not open " << filename;
		exit(1);
	}

	inputAdjacencies(adjacenciesFile);
	fclose(adjacenciesFile);

	//outputAdjacenciesMapData();
}


string adjacencyMapper::getAdjacencyFilename()
{
	string filename = Configuration::getV2DocumentsPath() + "/map/cache/adjacencies.bin";
	if (!Utils::DoesFileExist(filename))
	{
		LOG(LogLevel::Warning) << "Could not find " << filename << " - looking in install folder";
		filename = Configuration::getV2Path() + "/map/cache/adjacencies.bin";
		if (!Utils::DoesFileExist(filename))
		{
			LOG(LogLevel::Error) << "Could not find " << filename << ". Try running Vic2 and converting again.";
			exit(-1);
		}
	}

	return filename;
}


void adjacencyMapper::inputAdjacencies(FILE* adjacenciesFile)
{
	int current_province = 0;
	while (!feof(adjacenciesFile))
	{
		int numAdjacencies;
		if (fread(&numAdjacencies, sizeof(numAdjacencies), 1, adjacenciesFile) != 1)
		{
			break;
		}

		vector<int> adjacencies = readAnAdjacenciesSet(adjacenciesFile, numAdjacencies);
		adjacencyMap.insert(make_pair(current_province, adjacencies));
		current_province++;
	}
}


typedef struct {
	int type;			// the type of adjacency 0 = normal, 1 = ford, 2 = river crossing
	int to;				// the province this one is adjacent to (expect one pointing back to this province)
	int via;				// the straight (if any) this crosses
	int unknown1;		// still unknown
	int unknown2;		// still unknown
	int pathX;			// the midpoint on the path drawn between provinces
	int pathY;			// the midpoint on the path drawn between provinces
	int unknown3;		// still unknown
	int unknown4;		// still unknown
} HODAdjacency;		// an entry in the HOD adjacencies.bin format

typedef struct {
	int type;			// the type of adjacency 0 = normal, 1 = ford, 2 = river crossing
	int to;				// the province this one is adjacent to (expect one pointing back to this province)
	int via;				// the straight (if any) this crosses
	int unknown1;		// still unknown
	int unknown2;		// still unknown
	int pathX;			// the midpoint on the path drawn between provinces
	int pathY;			// the midpoint on the path drawn between provinces
} AHDAdjacency;		// an entry in the AHD adjacencies.bin format

typedef struct {
	int type;			// the type of adjacency 0 = normal, 1 = ford, 2 = river crossing
	int to;				// the province this one is adjacent to (expect one pointing back to this province)
	int via;				// the straight (if any) this crosses
	int unknown1;		// still unknown
	int unknown2;		// still unknown
} VanillaAdjacency;	// an entry in the vanilla adjacencies.bin format

vector<int> adjacencyMapper::readAnAdjacenciesSet(FILE* adjacenciesFile, int numAdjacencies)
{
	vector<int> adjacencies;
	for (int i = 0; i < numAdjacencies; i++)
	{
		if (Configuration::getV2Gametype() == "vanilla")
		{
			VanillaAdjacency readAdjacency;
			fread(&readAdjacency, sizeof(readAdjacency), 1, adjacenciesFile);
			adjacencies.push_back(readAdjacency.to);
		}
		else if (Configuration::getV2Gametype() == "AHD")
		{
			AHDAdjacency readAdjacency;
			fread(&readAdjacency, sizeof(readAdjacency), 1, adjacenciesFile);
			adjacencies.push_back(readAdjacency.to);
		}
		if ((Configuration::getV2Gametype() == "HOD") || (Configuration::getV2Gametype() == "HoD-NNM"))
		{
			HODAdjacency readAdjacency;
			fread(&readAdjacency, sizeof(readAdjacency), 1, adjacenciesFile);
			adjacencies.push_back(readAdjacency.to);
		}
	}

	return adjacencies;
}


void adjacencyMapper::outputAdjacenciesMapData()
{
	ofstream adjacenciesData("adjacenciesData.csv");

	adjacenciesData << "From,To\n";
	for (auto adjacencyMapping: adjacencyMap)
	{
		for (auto adjacency: adjacencyMapping.second)
		{
			adjacenciesData << adjacencyMapping.first << "," << adjacency << "\n";
		}
	}

	adjacenciesData.close();
}


vector<int> adjacencyMapper::GetVic2Adjacencies(int Vic2Province)
{
	if(adjacencyMap.find(Vic2Province) != adjacencyMap.end())
		return adjacencyMap.at(Vic2Province);
}