#ifndef CK2TITLE_H_
#define CK2TITLE_H_

#include "..\Parsers\Object.h"
#include "CK2Character.h"



class CK2Title
{
	public:
		void				init(Object*, map<int, CK2Character*>);
		void				addLiege(CK2Title*);
		void				addVassal(CK2Title*);
		void				addToHRE();
		string			getTitleString();
		CK2Character*	getHolder();
		string			getLiegeString();
		CK2Title*		getLiege();
		bool				isIndependent();
		bool				isInHRE();
		~CK2Title();
	private:
		string				titleString;
		CK2Character*		holder;
		string				liegeString;
		CK2Title*			liege;
		vector<CK2Title*>	vassals;
		bool					independent;
		bool					inHRE;
};



#endif // CK2TITLE_H_