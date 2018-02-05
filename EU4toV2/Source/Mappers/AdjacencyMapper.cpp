/*Copyright (c) 2018 The Paradox Game Converters Project

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
#include <cstdint>



mappers::adjacencyMapper* mappers::adjacencyMapper::instance = nullptr;



mappers::adjacencyMapper::adjacencyMapper()
{
	LOG(LogLevel::Info) << "Importing province adjacencies";
	string filename = getAdjacencyFilename();

	ifstream adjacenciesFile(filename, std::ios_base::binary);
	if (!adjacenciesFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not open " << filename;
		exit(-1);
	}

	inputAdjacencies(adjacenciesFile);
	adjacenciesFile.close();

	if (Configuration::getDebug())
	{
		outputAdjacenciesMapData();
	}
}


std::string mappers::adjacencyMapper::getAdjacencyFilename()
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


void mappers::adjacencyMapper::inputAdjacencies(std::istream& adjacenciesFile)
{
	int current_province = 0;
	while (!adjacenciesFile.eof())
	{
		uint32_t numAdjacencies;
		adjacenciesFile.read(reinterpret_cast<char*>(&numAdjacencies), 4);

		vector<int> adjacencies = readAnAdjacenciesSet(adjacenciesFile, numAdjacencies);
		adjacencyMap.insert(make_pair(current_province, adjacencies));
		current_province++;
	}
}


typedef struct
{
	uint32_t type;			// the type of adjacency 0 = normal, 1 = ford, 2 = river crossing
	uint32_t to;				// the province this one is adjacent to (expect one pointing back to this province)
	uint32_t via;				// the straight (if any) this crosses
	uint32_t unknown1;		// still unknown
	uint32_t unknown2;		// still unknown
	uint32_t pathX;			// the midpoint on the path drawn between provinces
	uint32_t pathY;			// the midpoint on the path drawn between provinces
	uint32_t unknown3;		// still unknown
	uint32_t unknown4;		// still unknown
} HODAdjacency;		// an entry in the HOD adjacencies.bin format

std::istream& operator >> (std::istream& stream, HODAdjacency& adjacency)
{
	stream.read(reinterpret_cast<char*>(&adjacency.type), 4);
	stream.read(reinterpret_cast<char*>(&adjacency.to), 4);
	stream.read(reinterpret_cast<char*>(&adjacency.via), 4);
	stream.read(reinterpret_cast<char*>(&adjacency.unknown1), 4);
	stream.read(reinterpret_cast<char*>(&adjacency.unknown2), 4);
	stream.read(reinterpret_cast<char*>(&adjacency.pathX), 4);
	stream.read(reinterpret_cast<char*>(&adjacency.pathY), 4);
	stream.read(reinterpret_cast<char*>(&adjacency.unknown3), 4);
	stream.read(reinterpret_cast<char*>(&adjacency.unknown4), 4);

	return stream;
};

typedef struct
{
	uint32_t type;			// the type of adjacency 0 = normal, 1 = ford, 2 = river crossing
	uint32_t to;				// the province this one is adjacent to (expect one pointing back to this province)
	uint32_t via;				// the straight (if any) this crosses
	uint32_t unknown1;		// still unknown
	uint32_t unknown2;		// still unknown
	uint32_t pathX;			// the midpoint on the path drawn between provinces
	uint32_t pathY;			// the midpoint on the path drawn between provinces
} AHDAdjacency;		// an entry in the AHD adjacencies.bin format

std::istream& operator >> (std::istream& stream, AHDAdjacency& adjacency)
{
	stream.read(reinterpret_cast<char*>(&adjacency.type), 4);
	stream.read(reinterpret_cast<char*>(&adjacency.to), 4);
	stream.read(reinterpret_cast<char*>(&adjacency.via), 4);
	stream.read(reinterpret_cast<char*>(&adjacency.unknown1), 4);
	stream.read(reinterpret_cast<char*>(&adjacency.unknown2), 4);
	stream.read(reinterpret_cast<char*>(&adjacency.pathX), 4);
	stream.read(reinterpret_cast<char*>(&adjacency.pathY), 4);

	return stream;
}

typedef struct
{
	uint32_t type;			// the type of adjacency 0 = normal, 1 = ford, 2 = river crossing
	uint32_t to;				// the province this one is adjacent to (expect one pointing back to this province)
	uint32_t via;				// the straight (if any) this crosses
	uint32_t unknown1;		// still unknown
	uint32_t unknown2;		// still unknown
} VanillaAdjacency;	// an entry in the vanilla adjacencies.bin format

std::istream& operator >> (std::istream& stream, VanillaAdjacency& adjacency)
{
	stream.read(reinterpret_cast<char*>(&adjacency.type), 4);
	stream.read(reinterpret_cast<char*>(&adjacency.to), 4);
	stream.read(reinterpret_cast<char*>(&adjacency.via), 4);
	stream.read(reinterpret_cast<char*>(&adjacency.unknown1), 4);
	stream.read(reinterpret_cast<char*>(&adjacency.unknown2), 4);

	return stream;
}


std::vector<int> mappers::adjacencyMapper::readAnAdjacenciesSet(std::istream& adjacenciesFile, unsigned int numAdjacencies)
{
	vector<int> adjacencies;
	for (unsigned int i = 0; i < numAdjacencies; i++)
	{
		if (Configuration::getV2Gametype() == "vanilla")
		{
			VanillaAdjacency readAdjacency;
			adjacenciesFile >> readAdjacency;
			adjacencies.push_back(readAdjacency.to);
		}
		else if (Configuration::getV2Gametype() == "AHD")
		{
			AHDAdjacency readAdjacency;
			adjacenciesFile >> readAdjacency;
			adjacencies.push_back(readAdjacency.to);
		}
		if ((Configuration::getV2Gametype() == "HOD") || (Configuration::getV2Gametype() == "HoD-NNM"))
		{
			HODAdjacency readAdjacency;
			adjacenciesFile >> readAdjacency;
			adjacencies.push_back(readAdjacency.to);
		}
	}

	return adjacencies;
}


void mappers::adjacencyMapper::outputAdjacenciesMapData()
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


std::optional<std::vector<int>> mappers::adjacencyMapper::GetVic2Adjacencies(int Vic2Province)
{
	if(adjacencyMap.find(Vic2Province) != adjacencyMap.end())
	{
		return adjacencyMap.at(Vic2Province);
	}
	else
	{
		return {};
	}
}