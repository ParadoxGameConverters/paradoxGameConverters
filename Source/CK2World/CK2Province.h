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
		CK2Province();
		void						init(Object*, map<string, CK2Title*>);
		int						getNumber();
		vector<CK2Barony*>	getBaronies();
	private:
		int						number;
		vector<CK2Barony*>	baronies;
};



#endif // CK2PROVINCE_H_