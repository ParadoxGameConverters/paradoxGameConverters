#ifndef CK2PROVINCE_H_
#define CK2PROVINCE_H_


#include <vector>
#include <map>
using namespace std;



class Object;
class CK2Title;
class CK2Barony;

class CK2Province
{
	public:
		CK2Province(Object*, map<string, CK2Title*>);

		int						getNumber()		const { return number; };
		vector<CK2Barony*>	getBaronies()	const { return baronies; };
		string					getCulture()	const { return culture; };
	private:
		int						number;
		vector<CK2Barony*>	baronies;
		string					culture;
};



#endif // CK2PROVINCE_H_