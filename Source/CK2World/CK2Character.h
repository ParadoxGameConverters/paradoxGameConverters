#ifndef CK2CHARACTER_H_
#define CK2CHARACTER_H_


#include <string>
#include <list>
#include <map>
#include <vector>
#include "..\Date.h"
using namespace std;



class		Object;
struct	date;
class		CK2Dynasty;
class		CK2Trait;

class CK2Character
{
	public:
		CK2Character();
		void				init(Object*, map<int, CK2Dynasty*>&, map<int, CK2Trait*>&, date theDate);
		string			getName();
		CK2Dynasty*		getDynasty();
		date				getBirthDate();
		void				setParents(map<int, CK2Character*>&);
		void				addChild(CK2Character*);
		bool				isDead();
		date				getDeathDate();
		bool				isFemale();
		bool				isBastard();
		int*				getStats();
		CK2Character*	getFather();
		CK2Character*	getPrimogenitureHeir(string);
	private:
		string		name;
		string		religion;
		string		culture;
		CK2Dynasty*	dynasty;
		date			birthDate;
		float			age;
		bool			dead;
		date			deathDate;
		bool			female;
		bool			bastard;
		vector<int>	traits;
		int			stats[5];

		int							fatherNum;
		CK2Character*				father;
		int							motherNum;
		CK2Character*				mother;
		list<CK2Character*>		children;
		int							guardianNum;
		CK2Character*				guardian;
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