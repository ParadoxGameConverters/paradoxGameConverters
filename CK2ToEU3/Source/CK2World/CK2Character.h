/*Copyright (c) 2013 The CK2 to EU3 Converter Project
 
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



#ifndef CK2CHARACTER_H_
#define CK2CHARACTER_H_


#include <string>
#include <list>
#include <map>
#include <vector>
#include "..\Date.h"
#include "..\Mappers.h"
#include "CK2Opinion.h"
using namespace std;



class		Object;
class		CK2Dynasty;
class		CK2Trait;
class		CK2Province;
class		CK2Barony;
class		CK2Title;
class		CK2War;
class		CK2Religion;
class		CK2Army;
class		CK2Version;

enum advisorTypes
{
	CHANCELLOR	= 0,
	MARSHAL,
	STEWARD,
	SPYMASTER,
	CHAPLAIN,
	NONE,
};


class CK2Character
{
	public:
		CK2Character(Object*, const map<int, CK2Dynasty*>&, const map<int, CK2Trait*>&, date theDate);
		void							readOpinionModifiers(Object* obj);
	
		void							addTitle(CK2Title*);
		void							removeTitle(CK2Title*);
		void							setParents(map<int, CK2Character*>&);
		void							setEmployer(map<int, CK2Character*>&, map<string, CK2Barony*>&);
		void							setGavelkindHeirs(string);
		void							setStateStats();

		CK2Character*				getPrimogenitureHeir(string, CK2Character*);
		CK2Character*				getUltimogenitureHeir(string, CK2Character*);
		vector<CK2Character*>	getPotentialOpenHeirs(string, CK2Character*);

		void							mergeTitles(bool useInheritance);

		void							setCapital(CK2Province* _capital)					{ capital = _capital; };
		void							setPrimaryHolding(CK2Barony* _primaryHolding)	{ primaryHolding = _primaryHolding; };
		void							setPrimaryTitle(const map<string, CK2Title*>& titleMap);
		void							addHolding(CK2Barony* holding)					{ holdings.push_back(holding); };
		void							addWar(CK2War* war)								{ wars.push_back(war); };

		int							getNum()						const { return num; };
		string						getName()					const { return name; };
		string						getCulture()				const { return culture; };
		CK2Religion*				getReligion()				const { return religion; };
		CK2Dynasty*					getDynasty()				const { return dynasty; };
		date							getBirthDate()				const { return birthDate; };
		bool							isDead()						const { return dead; };
		date							getDeathDate()				const { return deathDate; };
		bool							isFemale()					const { return female; };
		bool							isBastard()					const { return bastard; };
		int*							getStats()					const { return (int*)stats; };
		int*							getStateStats()			const { return (int*)stateStats; };
		vector<CK2Title*>			getTitles()					const { return titles; };
		CK2Title*					getPrimaryTitle()			const { return primaryTitle; }
		CK2Character*				getFather()					const { return father; };
		CK2Character*				getRegent()					const { return regent; };
		CK2Character**				getAdvisors()				const { return (CK2Character**)advisors; };
		advisorTypes				getJobType()				const { return jobType; };
		string						getAction()					const { return action; };
		int							getLocationNum()			const { return locationNum; };
		string						getCapitalString()		const { return capitalString; };
		CK2Province*				getCapital()				const { return capital; };
		CK2Barony*					getPrimaryHolding()		const { return primaryHolding; };
		vector<CK2Character*>	getSpouses()				const { return spouses; };
		CK2Character*				getPrimarySpouse()		const;
		vector<CK2Character*>	getCloseRelations()		const;
		bool							hasTrait(string traitName) const;
		int							getDemesneCap(CK2Version& version)			const;
		double						getTotalScore()			const { return prestige + piety + score; };
		vector<CK2Army*>			getArmies()					const { return armies; };
		vector<CK2Army*>			getNavies()					const { return navies; };

		bool						isCloseRelationOf(const CK2Character* other) const;
		bool						isRMWith(const CK2Character* other) const;
		bool						isAlliedWith(const CK2Character* other) const;
		int						getOpinionOf(const CK2Character* other, CK2Version& version) const;
		bool						isDirectVassalOf(const CK2Character* other) const;
	private:
		vector<CK2Character*>	getGavelkindHeirs(string);

		void							addChild(CK2Character*);
		void							addAdvisor(CK2Character*, advisorTypes);

		int					num;
		string				name;
		CK2Religion*		religion;
		string				culture;
		CK2Dynasty*			dynasty;
		date					birthDate;
		float					age;
		bool					dead;
		date					deathDate;
		bool					female;
		bool					bastard;
		vector<int>			traitNums;
		vector<CK2Trait*>	traits;
		int					stats[5];
		int					stateStats[5];
		vector<CK2Title*>	titles;
		double				piety;
		double				prestige;
		double				score;

		int							fatherNum;
		CK2Character*				father;
		int							motherNum;
		CK2Character*				mother;
		list<CK2Character*>		children;
		int							guardianNum;
		CK2Character*				guardian;
		int							regentNum;
		CK2Character*				regent;
		vector<int>					spouseNums;
		vector<CK2Character*>	spouses;

		CK2Character*				advisors[6];
		int							employerNum;
		advisorTypes				jobType;
		string						action;
		int							hostNum;
		int							locationNum;
		string						capitalString;
		CK2Province*				capital;
		CK2Barony*					primaryHolding;
		string						primaryTitleString;
		CK2Title*					primaryTitle;
		vector<CK2Army*>			armies;
		vector<CK2Army*>			navies;
		vector<CK2Barony*>		holdings;
		vector<CK2War*>			wars;
		
		map<int, vector<CK2Opinion> >		opinionMods;
};


enum stats
{
	DIPLOMACY = 0,
	MARTIAL,
	STEWARDSHIP,
	INTRIGUE,
	LEARNING
};







#endif // CK2CHARACTER_H_