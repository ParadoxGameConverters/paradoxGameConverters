#ifndef EU3ARMY_H_
#define EU3ARMY_H_


#include "..\Mappers.h"
#include <fstream>
#include <map>
#include <vector>
using namespace std;


class EU3Province;



typedef enum
{
	// ground units
	infantry = 0,
	cavalry,
	artillery,
	// navy units
	big_ship,
	light_ship,
	galley,
	transport,
	// end of enum
	num_reg_categories
} RegimentCategory;


static const char* RegimentCategoryNames[] = {
	"infantry",
	"cavalry",
	"artillery",
	"big_ship",
	"light_ship",
	"galley",
	"transport"
};


typedef map<string, pair<RegimentCategory, int> > RegimentTypeMap;


class CK2Subunit;
class CK2Army;


class EU3Regiment // also Ship
{
	public:
		EU3Regiment(const string _type, const double _strength);
		void	output(FILE*);

		void	setHomeProvince(int _home)		{ home = _home; };
		void	setName(string _name)			{ name = _name; };
	private:
		int						id;
		string					name;
		int						home;
		string					type;
		double					morale;
		double					strength;
};


class EU3Army // also Navy
{
	public:
		EU3Army(const CK2Army* srcArmy, const inverseProvinceMapping inverseProvinceMap, const string infantryType, const string cavalryType, map<int, EU3Province*> provinces, double& manpower);

		int	getNumRegiments()	const { return regiments.size(); };

		void		output(FILE*);
	private:
		int						id;
		string					name;
		//int						previous;
		double					movementProgress;
		vector<int>				path;
		int						location;	
		vector<EU3Regiment*>	regiments;
		//int						stagingProvince;
		//int						base;
		//int						at_sea;

		//int						leaderID;
};



#endif // EU3ARMY_H_