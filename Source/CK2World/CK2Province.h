#ifndef CK2PROVINCE_H_
#define CK2PROVINCE_H_


#include "..\Mappers.h"
#include <vector>
#include <map>
using namespace std;



class Object;
class CK2Title;
class CK2BuildingFactory;
class CK2Barony;
class CK2Religion;
class CK2Character;
class CK2Version;

class CK2Province
{
	public:
		CK2Province(Object*, map<string, CK2Title*>& titles, map<int, CK2Character*>& characters, const CK2BuildingFactory* buildingFactory, CK2Version& version);

		int						getNumber()				const { return number; };
		vector<CK2Barony*>	getBaronies()			const { return baronies; };
		string					getCulture()			const { return culture; };
		CK2Religion*			getReligion()			const { return religion; };
		vector<double>			getTechLevels()		const { return techLevels; } ;
		bool						hasTradePost()			const { return tradePost; };
		int						getTpGarrisonSize()	const { return tpGarrisonSize; };
		CK2Title*				getTPOwner()			const { return tpOwner; };
	private:
		int						number;
		bool						tradePost;
		int						tpGarrisonSize;
		CK2Title*				tpOwner;
		vector<CK2Barony*>	baronies;
		string					culture;
		CK2Religion*			religion;
		vector<double>			techLevels;
};



#endif // CK2PROVINCE_H_