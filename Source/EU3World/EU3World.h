#include <fstream>
#include "..\Mappers.h"
#include "EU3Province.h"
#include "..\Date.h"
#include	"..\CK2World\CK2World.h"



class EU3World
{
	public:
		void	output(FILE*);
		void	init(CK2World);
		void	setupRotwProvinces(inverseProvinceMapping);

	private:
		date						startDate;
		vector<EU3Province>	provinces;
};