#ifndef CK2PROVINCE_H_
#define CK2PROVINCE_H_


#include <vector>
#include "..\Parsers\Object.h"
#include "CK2Title.h"
#include "CK2Barony.h"



class CK2Province
{
	public:
		void						init(Object*, map<string, CK2Title*>);
		vector<CK2Barony*>	getBaronies();
	private:
		int						number;
		vector<CK2Barony*>	baronies;
};



#endif // CK2PROVINCE_H_