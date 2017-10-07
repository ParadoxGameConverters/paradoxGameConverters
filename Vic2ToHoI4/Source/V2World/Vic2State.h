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
		Vic2State(shared_ptr<Object> stateObj, const string& ownerTag);

		void determineEmployedWorkers();
		void determineIfPartialState();

		int getPopulation() const;
		int getAverageRailLevel() const;

		void addProvince(const V2Province* province) { provinces.insert(province); }

		set<const V2Province*> getProvinces() const { return provinces; }
		set<int> getProvinceNums() const { return provinceNums; }
		string getOwner() const { return owner; }
		string getStateID() const { return stateID; }
		bool isPartialState() const { return partialState; }
		int getEmployedWorkers() const { return employedWorkers; }

	private:
		Vic2State(const Vic2State&) = delete;
		Vic2State& operator=(const Vic2State&) = delete;

		void addProvinceNums(shared_ptr<Object> stateObj);
		void setID();

		void setFactoryLevel(shared_ptr<Object> stateObj);

		workerStruct countEmployedWorkers() const;
		workerStruct limitWorkersByFactoryLevels(const workerStruct& workers) const;
		int determineEmployedWorkersScore(const workerStruct& workers) const;
		bool ownerHasNoCores() const;


		const string owner;
		string stateID;
		bool partialState;

		set<int> provinceNums;
		set<const V2Province*> provinces;

		int factoryLevel;
		int employedWorkers;
};



#endif // VIC2STATE_H_
