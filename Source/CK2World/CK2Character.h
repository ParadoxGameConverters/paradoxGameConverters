#ifndef CK2CHARACTER_H_
#define CK2CHARACTER_H_


#include <string>
#include <list>
#include "..\Parsers\Object.h"
#include "..\Date.h"
#include "CK2Dynasty.h"
using namespace std;


class CK2Character
{
	public:
		CK2Character();
		void				init(Object*, map<int, CK2Dynasty*>&);
		string			getName();
		CK2Dynasty*		getDynasty();
		date				getBirthDate();
		void				setParents(map<int, CK2Character*>&);
		void				addChild(CK2Character*);
		bool				isDead();
		date				getDeathDate();
		bool				isFemale();
		int*				getStats();
		CK2Character*	getFather();
		CK2Character*	getPrimogenitureHeir();
	private:
		string		name;
		string		religion;
		string		culture;
		CK2Dynasty*	dynasty;
		date			birthDate;
		bool			dead;
		date			deathDate;
		bool			female;
		int			stats[5];

		int							fatherNum;
		CK2Character*				father;
		int							motherNum;
		CK2Character*				mother;
		list<CK2Character*>		children;
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