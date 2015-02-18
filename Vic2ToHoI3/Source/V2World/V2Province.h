/*Copyright (c) 2014 The Paradox Game Converters Project

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
#include <string>
#include <vector>
#include <map>
using namespace std;

class V2Country;
class V2Pop;
class V2Factory;


class V2Province
{
	public:
		V2Province(string _filename);
		V2Province(Object* obj);
		void output() const;
		void determineColonial();
		void addCore(string);
		void						removeCore(string tag);
		void addOldPop(const V2Pop*);
		void sortPops();

		int				getTotalPopulation() const;
		vector<V2Pop*>	getPops(string type) const;
		int				getSoldierPopForArmy(bool force = false);
		pair<int, int>	getAvailableSoldierCapacity() const;
		bool				hasCulture(string culture, float percentOfPopulation) const;
		
		void				clearCores()								{ cores.clear(); };
		void				setCoastal(bool _coastal)				{ coastal = _coastal; };
		void				setName(string _name)					{ name = _name; };
		void				setOwnerString(string _ownerString)					{ ownerString = _ownerString; };
		void				setOwner(V2Country* _owner)					{ owner = _owner; };
		void				setLandConnection(bool _connection)	{ landConnection = _connection; };
		void				setSameContinent(bool _same)			{ sameContinent = _same; };
		void				setFortLevel(int level)					{ fortLevel = level; };
		void				setNavalBaseLevel(int level)			{ navalBaseLevel = level; };
		void				setRailLevel(int level)					{ railLevel = level; };

		int						getOldPopulation()	const	{ return oldPopulation; };
		bool						wasInfidelConquest()	const { return originallyInfidel; };
		bool						wasColonised()			const { return colonised; };
		bool						isColonial()			const { return colonial != 0; };
		string					getRgoType()			const { return rgoType; };
		string					getOwnerString()	const { return ownerString; };
		V2Country*					getOwner()				const { return owner; };
		int						getNum()					const { return num; };
		string					getName()				const { return name; };
		bool						isCoastal()				const { return coastal; };
		bool						hasNavalBase()			const { return (navalBaseLevel > 0); };

		vector<V2Country*>	getCores(const map<string, V2Country*>& countries) const;
		int					getInfra() const { return railLevel; };
		int					getFort() const { return fortLevel; };
		int					getNavalBase() const { return navalBaseLevel; };
		double				getAvgMil() const;
		int					getPopulation(string type = "") const;
		int					getLiteracyWeightedPopulation(string type = "") const;

	private:
		void outputPops(FILE*) const;
		void combinePops();
		bool growSoldierPop(V2Pop* pop);

		string							filename;
		bool								coastal;
		int								num;
		string							name;
		string								ownerString;			// a string with the owner's tag
		V2Country*							owner;
		vector<string>					cores;
		int								colonyLevel;
		int								colonial;
		bool								colonised;
		bool								landConnection;
		bool								sameContinent;
		bool								originallyInfidel;
		int								oldPopulation;
		vector<const V2Pop*>			oldPops;
		vector<V2Pop*>					pops;
		string							rgoType;
		string							terrain;
		int								lifeRating;
		bool								slaveState;
		int								fortLevel;
		int								navalBaseLevel;
		int								railLevel;
		vector<const V2Factory*>	factories;
};



#endif // V2PROVINCE_H_