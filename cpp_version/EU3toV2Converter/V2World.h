#ifndef V2WORLD_H_
#define V2WORLD_H_

#include <vector>
#include "V2Province.h"
#include "V2Country.h"
#include "V2State.h"
#include "EU3World.h"
#include "Parsers/Object.h"
#include "Mapper.h"
#include "Log.h"
using namespace std;


class V2World {
	public:
		void	init(Object* obj);
		void	convertProvinces(EU3World sourceWorld, provinceMapping provinceMap, countryMapping countryMap);
		void	setupStates(stateMapping);
	private:
		vector<V2Province>	provinces;
		vector<V2Country>		countries;
};


#endif // V2WORLD_H_