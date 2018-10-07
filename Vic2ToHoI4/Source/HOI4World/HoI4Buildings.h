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



#ifndef HOI4_BUILDINGS_H_
#define HOI4_BUILDINGS_H_



#include <fstream>
#include <map>
#include <optional>
#include <regex>
#include <set>
#include <string>
#include <vector>



class HoI4States;


namespace HoI4
{

class coastalProvinces;
class MapData;


struct buildingPosition
{
	double xCoordinate = 0.0;
	double yCoordinate = 0.0;
	double zCoordinate = 0.0;
	double rotation = 0.0;
};


class Building
{
	public:
		explicit Building(int _stateID, const std::string& _type, buildingPosition& _position, std::optional<int> _connectingSeaProvince);

		friend std::ostream& operator << (std::ostream& out, const Building& building);

	private:
		Building(const Building&) = delete;
		Building& operator=(const Building&) = delete;

		int stateID = 0;
		std::string type;
		buildingPosition position;
		int connectingSeaProvince = 0;
};


std::ostream& operator << (std::ostream& out, const Building& building);


typedef std::map<std::pair<int, int>, buildingPosition> defaultPositions;


class Buildings
{
	public:
		explicit Buildings(const HoI4States& theStates, const coastalProvinces& theCoastalProvinces, MapData& theMapData);

		void output() const;

	private:
		Buildings(const Buildings&) = delete;
		Buildings& operator=(const Buildings&) = delete;

		void importDefaultBuildings(MapData& theMapData);
		void processLine(const std::string& line, MapData& theMapData);
		void importDefaultBuilding(const std::smatch& matches, defaultPositions& positions, MapData& theMapData);

		void placeBuildings(const HoI4States& theStates, const coastalProvinces& theCoastalProvinces, const MapData& theMapData);
		void placeArmsFactories(const HoI4States& theStates, const MapData& theMapData);
		void placeIndustrialComplexes(const HoI4States& theStates, const MapData& theMapData);
		void placeAirports(const HoI4States& theStates, const MapData& theMapData);
		void placeNavalBases(const std::map<int, int>& provinceToStateIDMap, std::map<int, std::vector<int>> actualCoastalProvinces, const MapData& theMapData);
		void addNavalBase(int stateID, const std::pair<int, std::vector<int>>& province, const MapData& theMapData);
		void placeBunkers(const std::map<int, int>& provinceToStateIDMap, const MapData& theMapData);
		void addBunker(int stateID, int province, const MapData& theMapData);
		void placeCoastalBunkers(const std::map<int, int>& provinceToStateIDMap, std::map<int, std::vector<int>> actualCoastalProvinces, const MapData& theMapData);
		void addCoastalBunker(int stateID, const std::pair<int, std::vector<int>>& province, const MapData& theMapData);
		void placeDockyards(const HoI4States& theStates, const coastalProvinces& theCoastalProvinces, std::map<int, std::vector<int>> actualCoastalProvinces, const MapData& theMapData);
		void placeAntiAir(const HoI4States& theStates, const MapData& theMapData);
		void placeSyntheticRefineries(const HoI4States& theStates, const MapData& theMapData);
		void placeNuclearReactors(const HoI4States& theStates, const MapData& theMapData);

		std::multimap<int, Building*> buildings;

		defaultPositions defaultArmsFactories;
		defaultPositions defaultIndustrialComplexes;
		defaultPositions defaultAirBases;
		defaultPositions defaultNavalBases;
		defaultPositions defaultBunkers;
		defaultPositions defaultCoastalBunkers;
		defaultPositions defaultDockyards;
		defaultPositions defaultAntiAirs;
		defaultPositions defaultSyntheticRefineries;
		defaultPositions defaultNuclearReactors;

		std::map<int, int> airportLocations;
};

}




#endif // HOI4_BUILDINGS_H_