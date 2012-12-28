#ifndef CK2CHARACTER_H_
#define CK2CHARACTER_H_


#include <string>
#include <list>
#include <map>
#include <vector>
#include "..\Date.h"
#include "CK2Opinion.h"
using namespace std;



class		Object;
class		CK2Dynasty;
class		CK2Trait;
class		CK2Province;
class		CK2Barony;
class		CK2Title;

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
		CK2Character(Object*, map<int, CK2Dynasty*>&, map<int, CK2Trait*>&, date theDate);
		void							readOpinionModifiers(Object* obj);
	
		void							addTitle(CK2Title*);
		void							removeTitle(CK2Title*);
		void							setParents(map<int, CK2Character*>&);
		void							setEmployer(map<int, CK2Character*>&, map<string, CK2Barony*>&);
		void							setGavelkindHeirs(string);

		CK2Character*				getPrimogenitureHeir(string, CK2Character*);
		vector<CK2Character*>	getPotentialOpenHeirs(string, CK2Character*);

		void							mergeTitles(bool useInheritance);

		void							setCapital(CK2Province* _capital)					{ capital = _capital; };
		void							setPrimaryHolding(CK2Barony* _primaryHolding)	{ primaryHolding = _primaryHolding; };
		void							setPrimaryTitle(const map<string, CK2Title*>& titleMap);

		int							getNum()						const { return num; };
		string						getName()					const { return name; };
		string						getCulture()				const { return culture; };
		string						getReligion()				const { return religion; };
		CK2Dynasty*					getDynasty()				const { return dynasty; };
		date							getBirthDate()				const { return birthDate; };
		bool							isDead()						const { return dead; };
		date							getDeathDate()				const { return deathDate; };
		bool							isFemale()					const { return female; };
		bool							isBastard()					const { return bastard; };
		int*							getStats()					const { return (int*)stats; };
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
		vector<CK2Character*>		getSpouses()			const { return spouses; };
		vector<CK2Character*>		getCloseRelations() const;

		bool						isCloseRelationOf(CK2Character* other) const;
		bool						isRMWith(CK2Character* other) const;
		bool						isAlliedWith(CK2Character* other) const;
		int							getOpinionOf(CK2Character* other) const;
	private:
		vector<CK2Character*>	getGavelkindHeirs(string);

		void							addChild(CK2Character*);
		void							addAdvisor(CK2Character*, advisorTypes);

		int					num;
		string				name;
		string				religion;
		string				culture;
		CK2Dynasty*			dynasty;
		date					birthDate;
		float					age;
		bool					dead;
		date					deathDate;
		bool					female;
		bool					bastard;
		vector<int>			traits;
		int					stats[5];
		vector<CK2Title*>	titles;

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
		vector<CK2Character*>		spouses;

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