#ifndef CK2TITLE_H_
#define CK2TITLE_H_

#include "..\Parsers\Object.h"



class CK2Title
{
	public:
		void			init(Object*);
		void			addLiege(CK2Title*);
		void			addVassal(CK2Title*);
		void			addToHRE();
		string		getTitleString();
		string		getLiegeString();
		CK2Title*	getLiege();
		bool			isIndependent();
		bool			isInHRE();
		~CK2Title();
	private:
		string				titleString;
		string				liegeString;
		CK2Title*			liege;
		vector<CK2Title*>	vassals;
		bool					independent;
		bool					inHRE;
};



#endif // CK2TITLE_H_