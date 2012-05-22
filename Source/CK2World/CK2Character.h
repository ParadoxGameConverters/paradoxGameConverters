#ifndef CK2CHARACTER_H_
#define CK2CHARACTER_H_


#include <string>
#include <list>
#include <map>
#include <vector>
#include "..\Date.h"
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
		CK2Character();
		void				init(Object*, map<int, CK2Dynasty*>&, map<int, CK2Trait*>&, date theDate);
		int				getNum();
		string			getName();
		CK2Dynasty*		getDynasty();
		date				getBirthDate();
		void				addTitle(CK2Title*);
		int				getDemesneSize();
		void				setParents(map<int, CK2Character*>&);
		void				setEmployer(map<int, CK2Character*>&, map<string, CK2Barony*>&);
		void				addChild(CK2Character*);
		void				addAdvisor(CK2Character*, advisorTypes);
		CK2Character**	getAdvisors();
		bool				isDead();
		date				getDeathDate();
		bool				isFemale();
		bool				isBastard();
		int*				getStats();
		CK2Character*	getFather();
		CK2Character*	getPrimogenitureHeir(string);
		void				setGavelkindHeirs(string);
		advisorTypes	getJobType();
		string			getAction();
		int				getLocationNum();
		string			getCapitalString();
	private:
		vector<CK2Character*>	getGavelkindHeirs(string);

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

		CK2Character*				advisors[6];
		int							employerNum;
		advisorTypes				jobType;
		string						action;
		int							hostNum;
		int							locationNum;
		string						capitalString;
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