#ifndef EU3COUNTRY_H_
#define EU3COUNTRY_H_

#include <string>
#include <vector>
#include "Parsers\Object.h"
#include "EU3Province.h"
using namespace std;


class EU3Country
{
	public:
		void						init(Object* obj);
		string					getTag();
		void						addProvince(EU3Province*);
		vector<EU3Province*>	getProvinces();
	private:
		string					tag;
		vector<EU3Province*>	provinces;
};


#endif // EU3COUNTRY_H_