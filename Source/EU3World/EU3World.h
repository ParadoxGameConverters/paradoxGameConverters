#ifndef EU3WORLD_H_
#define EU3WORLD_H_


#include <fstream>
#include "..\Mappers.h"
#include "..\Date.h"



class CK2Province;
class CK2World;
class EU3Country;
class EU3Province;
class EU3Advisor;

class EU3World
{
	public:
		void						output(FILE*);
		void						init(CK2World*);
		void						convertProvinces(provinceMapping, map<int, CK2Province*>, countryMapping);
		void						convertAdvisors(inverseProvinceMapping, countryMapping);
		void						setupRotwProvinces(provinceMapping);
		void						addPotentialCountries();
		vector<EU3Country*>	getCountries();
	private:
		date							startDate;
		map<int, EU3Province*>	provinces;
		vector<EU3Country*>		countries;
		vector<string>				europeanCountries;
		vector<EU3Advisor*>		advisors;
};



#endif	// EU3WORLD_H_