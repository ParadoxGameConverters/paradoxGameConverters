/*Copyright (c) 2016 The Paradox Game Converters Project

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



#include "../Mapper.h"
#include "Object.h"
#include <vector>
using namespace std;



class Vic2State
{
	public:
		Vic2State(const Object* stateObj, string ownerTag);

		void determinePartialState(const stateMapping& stateMap);

		void setID(const string& id) { stateID = id; }

		set<int>	getProvinces() const { return provinces; }
		string getOwner() const { return owner; }
		int getFactoryLevel() const { return factoryLevel; }
		string getStateID() const { return stateID; }
		bool isPartialState() const { return partialState; }

	private:
		void addProvinces(const Object* stateObj);
		vector<string> getProvinceIDs(const Object* stateObj);
		void setFactoryLevel(const Object* stateObj);
		void addBuildingLevel(const Object* buildingObj);

		set<int> provinces;
		string owner;
		int factoryLevel;
		string stateID;
		bool partialState;
};



#endif // VIC2STATE_H_
