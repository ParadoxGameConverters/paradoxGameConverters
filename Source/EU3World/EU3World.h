#include <fstream>
#include "..\Mappers.h"
#include "EU3Province.h"



class EU3World
{
	public:
		void	output(FILE*);
		void	setupRotwProvinces(inverseProvinceMapping);

	private:
		vector<EU3Province> provinces;
};