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
		void init(Object*, map<int, CK2Dynasty*>&);
		string		getName();
		CK2Dynasty*	getDynasty();
		date			getBirthDate();
		void			setParents(map<int, CK2Character*>&);
		void			addChild(CK2Character*);
	private:
		string		name;
		string		religion;
		string		culture;
		CK2Dynasty*	dynasty;
		date			birthDate;

		int							fatherNum;
		CK2Character*				father;
		int							motherNum;
		CK2Character*				mother;
		list<CK2Character*>		children;
};







#endif // CK2CHARACTER_H_