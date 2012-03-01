#include <fstream>
#include "..\Mappers.h"
#include "..\Date.h"
#include "EU3Country.h"
#include "EU3Province.h"
#include	"..\CK2World\CK2World.h"



class EU3World
{
	public:
		void				output(FILE*);
		void				init(CK2World);
		void				setupRotwProvinces(inverseProvinceMapping);
		void				addPotentialCountries(ifstream&, string);
		vector<string>	getPotentialTags();
	private:
		date						startDate;
		vector<EU3Province>	provinces;
		vector<EU3Country>	potentialCountries;
};