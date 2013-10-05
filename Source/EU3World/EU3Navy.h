#ifndef EU3NAVY_H_
#define EU3NAVY_H_


#include "..\Mappers.h"
#include <string>
using namespace std;



class CK2Subunit;
class CK2Army;
class EU3Province;
class EU3Army;



class EU3Ship
{
	public:
		EU3Ship(const string _type, const double _strength);
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


class EU3Navy // also Navy
{
	public:
		EU3Navy(const CK2Army* srcNavy, const inverseProvinceMapping inverseProvinceMap, const string transportType, const string infantryType, const string cavalryType, map<int, EU3Province*> provinces, double& manpower);
		EU3Navy(int numNavies, int numShips, int provNum, const string transportType, map<int, EU3Province*> provinces);

		int	getNumShips()	const { return ships.size(); };
		int	getLocation()	const { return location; };

		void		output(FILE*);
	private:
		int						id;
		string					name;
		double					movementProgress;
		vector<int>				path;
		int						location;	
		vector<EU3Ship*>		ships;
		bool						atSea;

		vector<EU3Army*>		transportedArmies;
};


#endif	// EU3NAVY_H_
