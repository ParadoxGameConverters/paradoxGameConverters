#ifndef CK2HISTORY_H_
#define CK2HISTORY_H_


#include "..\Date.h"
#include "..\Parsers\Object.h"
#include "CK2Character.h"




class CK2History
{
	public:
		void init(Object*, map<int, CK2Character*>&);
		date				getWhen();
		CK2Character*	getHolder();
	private:
		date				when;
		CK2Character*	holder;
};


#endif // CK2HISTORY_H_