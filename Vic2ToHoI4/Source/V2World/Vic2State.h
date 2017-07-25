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



#ifndef VIC2STATE_H_
#define VIC2STATE_H_



#include "../Mappers/Mapper.h"
#include "Object.h"
#include <set>
#include <vector>
using namespace std;


class V2Province;



typedef struct
{
	int craftsmen		= 0;
	int clerks			= 0;
	int artisans		= 0;
	int capitalists	= 0;
} workerStruct;



class Vic2State
{
	public:
		Vic2State(const shared_ptr<Object> stateObj, string ownerTag);

		void determineEmployedWorkers();
		void determineIfPartialState();

		int getPopulation() const;
		int getAverageRailLevel() const;

		void addProvince(V2Province* province) { provinces.insert(province); }

		const set<V2Province*>& getProvinces() const { return provinces; }
		const set<int>& getProvinceNums() const { return provinceNums; }
		const string& getOwner() const { return owner; }
		const string& getStateID() const { return stateID; }
		bool isPartialState() const { return partialState; }
		int getEmployedWorkers() const { return employedWorkers; }

	private:
		void addProvinceNums(const shared_ptr<Object> stateObj);
		void setID();
		vector<string> getProvinceIDs(const shared_ptr<Object> stateObj);

		void setFactoryLevel(const shared_ptr<Object> stateObj);
		void addBuildingLevel(const shared_ptr<Object> buildingObj);

		workerStruct countEmployedWorkers();
		workerStruct limitWorkersByFactoryLevels(workerStruct workers);
		int determineEmployedWorkersScore(workerStruct workers);
		bool ownerHasNoCores();


		string owner;
		string stateID;
		bool partialState;

		set<int> provinceNums;
		set<V2Province*> provinces;

		int factoryLevel;
		int employedWorkers;
};



#endif // VIC2STATE_H_
