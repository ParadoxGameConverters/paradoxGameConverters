#ifndef EU3WORLD_H_
#define EU3WORLD_H_


#include <fstream>
#include "..\Mappers.h"
#include "..\Date.h"
#include "EU3Country.h"
#include "EU3Province.h"
#include	"..\CK2World\CK2World.h"
#include "..\CK2World\CK2Province.h"



class EU3World
{
	public:
		void						output(FILE*);
		void						init(CK2World);
		void						convertProvinces(provinceMapping, map<int, CK2Province*>, countryMapping);
		void						setupRotwProvinces(provinceMapping);
		void						addPotentialCountries(ifstream&);
		vector<EU3Country*>	getCountries();
	private:
		date						startDate;
		vector<EU3Province>	provinces;
		vector<EU3Country*>	countries;
		vector<string>			europeanCountries;
};



#endif	// EU3WORLD_H_