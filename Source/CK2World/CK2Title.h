#ifndef CK2TITLE_H_
#define CK2TITLE_H_

#include "..\Parsers\Object.h"



class CK2Title
{
	public:
		void		init(Object*);
		void		addLiege(CK2Title*);
		void		addVassal(CK2Title*);
		string	getTitleString();
		string	getLiegeString();
	private:
		string				titleString;
		string				liegeString;
		CK2Title*			liege;
		vector<CK2Title*>	vassals;
};



#endif // CK2TITLE_H_