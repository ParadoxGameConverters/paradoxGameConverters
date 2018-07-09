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



namespace HoI4
{

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


typedef std::map<std::pair<int, int>, buildingPosition> defaultPositions;


class Buildings
{
	public:
		explicit Buildings(const std::map<int, int>& provinceToStateIDMap);

		void output() const;

	private:
		Buildings(const Buildings&) = delete;
		Buildings& operator=(const Buildings&) = delete;

		void importDefaultBuildings();
		void processLine(const std::string& line);
		void importDefaultBuilding(const std::smatch& matches, defaultPositions& positions);

		void placeCoastalBuildings(const std::map<int, int>& provinceToStateIDMap);
		void addNavalBase(int stateID, const std::pair<int, std::vector<int>>& province);
		void addCoastalBunker(int stateID, const std::pair<int, std::vector<int>>& province);

		std::multimap<int, Building*> buildings;

		defaultPositions defaultArmsFactories;
		defaultPositions defaultIndustrialComplexes;
		defaultPositions defaultAirBase;
		defaultPositions defaultNavalBases;
		defaultPositions defaultBunkers;
		defaultPositions defaultCoastalBunkers;
		defaultPositions defaultDockyards;
		defaultPositions defaultAntiAirs;
		defaultPositions defaultSyntheticRefineries;
		defaultPositions defaultNuclearReactors;
};

}




#endif // HOI4_BUILDINGS_H_