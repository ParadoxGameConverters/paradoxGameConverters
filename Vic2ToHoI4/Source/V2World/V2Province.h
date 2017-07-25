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



#ifndef V2PROVINCE_H_
#define V2PROVINCE_H_

#include "../Configuration.h"
#include "V2Rgo.h"
#include <map>
#include <set>
#include <string>
#include <vector>
using namespace std;

class Object;
class V2Country;
class V2Pop;

class V2Province
{
	public:
		V2Province(shared_ptr<Object> obj);
		void setCores(const map<string, V2Country*>& countries);

		int getTotalPopulation() const;
		int getPopulation(string type = "") const;
		int getLiteracyWeightedPopulation(string type = "") const;
		double getPercentageWithCultures(const set<string>& cultures) const;

		void setOwner(const V2Country* _owner) { owner = _owner; }
		void addCoreString(string coreString) { coreStrings.insert(coreString); }
		void removeCoreString(string coreString) { coreStrings.erase(coreString); }
		void removeCore(V2Country* core) { cores.erase(core); }

		int getNumber() const { return number; }
		string getOwnerString() const { return ownerString; }
		const V2Country* getOwner() const { return owner; }
		set<V2Country*> getCores() const { return cores; }
		int getRailLevel() const { return railLevel; }
		int getFortLevel() const { return fortLevel; }
		int getNavalBaseLevel() const { return navalBaseLevel; }
		const V2Rgo& getRgo() { return rgo; }

	private:
		void readOwner(shared_ptr<Object> obj);
		void readCores(shared_ptr<Object> obj);
		void readForts(shared_ptr<Object> obj);
		void readNavalBases(shared_ptr<Object> obj);
		void readRails(shared_ptr<Object> obj);
		void readRgo(shared_ptr<Object> obj);
		void readPops(shared_ptr<Object> obj);
		bool isPopObject(shared_ptr<Object> obj);
		int calculateLiteracyWeightedPop(const V2Pop* thePop) const;

		int number;

		string ownerString;
		const V2Country* owner;

		set<string> coreStrings;
		set<V2Country*> cores;

		vector<V2Pop*> pops;
		V2Rgo rgo;

		int fortLevel;
		int navalBaseLevel;
		int railLevel;
};



#endif // V2PROVINCE_H_
