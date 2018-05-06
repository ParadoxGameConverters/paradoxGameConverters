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



#ifndef VIC2_STATE_H_
#define VIC2_STATE_H_



#include "newParser.h"
#include <set>
#include <vector>



namespace Vic2
{

class Province;



typedef struct
{
	int craftsmen		= 0;
	int clerks			= 0;
	int artisans		= 0;
	int capitalists	= 0;
} workerStruct;



class State: commonItems::parser
{
	public:
		State(std::istream& theStream, const std::string& ownerTag);
		State(std::set<std::pair<int, Province*>> theProvinces);

		void determineEmployedWorkers();
		void determineIfPartialState();

		int getPopulation() const;
		int getAverageRailLevel() const;

		void addProvince(const Province* province) { provinces.insert(province); }

		std::set<const Province*> getProvinces() const { return provinces; }
		std::set<int> getProvinceNums() const { return provinceNums; }
		std::string getOwner() const { return owner; }
		std::string getStateID() const { return stateID; }
		bool isPartialState() const { return partialState; }
		int getEmployedWorkers() const { return employedWorkers; }

	private:
		State(const State&) = delete;
		State& operator=(const State&) = delete;

		void setID();

		workerStruct countEmployedWorkers() const;
		workerStruct limitWorkersByFactoryLevels(const workerStruct& workers) const;
		int determineEmployedWorkersScore(const workerStruct& workers) const;
		bool ownerHasNoCores() const;


		const std::string owner = "";
		std::string stateID = "";
		bool partialState = false;

		std::set<int> provinceNums;
		std::set<const Province*> provinces;

		int factoryLevel = 0;
		int employedWorkers = 0;
};

}



#endif // VIC2_STATE_H_
