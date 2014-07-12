/*Copyright(c) 2014 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */


#ifndef EU4PROVINCE_H_
#define EU4PROVINCE_H_


#include "../Date.h"
#include <string>
#include <vector>
#include <map>
using namespace std;

class Object;
class EU4Country;



struct EU4PopRatio {
	string culture;		// the culture
	string religion;		// the religion
	double popRatio;		// the percent of the total population this represents
};


class EU4Province {
	public:
		EU4Province(Object* obj);

		void						addCore(string tag);
		void						removeCore(string tag);
		bool						wasColonised() const;
		bool						wasInfidelConquest() const;
		bool						hasBuilding(string building) const;

		vector<EU4Country*>	getCores(const map<string, EU4Country*>& countries) const;
		date						getLastPossessedDate(string tag) const;

		int						getNum()				const { return num; };
		string					getOwnerString()	const { return ownerString; };
		EU4Country*				getOwner()			const { return owner; };
		int						getPopulation()	const { return population; };
		bool						isColony()			const { return colony; };
		vector<EU4PopRatio>	getPopRatios()		const { return popRatios; };

		void						setOwner(EU4Country* newOwner)	{ owner = newOwner; };
	private:
		void	checkBuilding(const Object* provinceObj, string building);
		void	buildPopRatios();
		void	decayPopRatios(date olddate, date newdate, EU4PopRatio& currentPop);

		int									num;						// the province number
		string								ownerString;			// a string with the owner's tag
		EU4Country*							owner;					// the owner
		vector<string>						cores;					// strings of the tags of all cores
		int									population;				// the population
		bool									colony;					// whether or not this is a colony
		vector< pair<date, string> >	ownershipHistory;		// the history of who has owned this province
		map<string, date>					lastPossessedDate;	// the last date the province was owned by different tags
		vector< pair<date, string> >	religionHistory;		// the history of the religious changes of this province
		vector< pair<date, string> >	cultureHistory;		// the history of the cultural changes of this province
		vector<EU4PopRatio>				popRatios;				// the population ratios of this province
		map<string, bool>					buildings;				// the buildings in this province
};


#endif // EU4PROVINCE_H_