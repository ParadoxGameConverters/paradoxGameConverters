#ifndef CK2ARMY_H_
#define CK2ARMY_H_


#include "..\Parsers\Object.h"
#include <string>
using namespace std;


class CK2Subunit
{
	public:
		CK2Subunit(const Object* obj);

		int		getHome()					const { return home; };
		double	getCurrentInfantryPSE()	const { return currentInfantryPSE; };
		double	getMaxInfantryPSE()		const { return maxInfantryPSE; };
		double	getCurrentCavalryPSE()	const { return currentCavalryPSE; };
		double	getMaxCavalryPSE()		const { return maxCavalryPSE; };
		double	getCurrentShips()			const { return currentShips; };
		double	getMaxShips()				const { return maxShips; };
	private:
		int		home;

		double	currentInfantryPSE;
		double	maxInfantryPSE;
		double	currentCavalryPSE;
		double	maxCavalryPSE;
		double	currentShips;
		double	maxShips;
};


class CK2Army
{
	public:
		CK2Army(const Object* obj);

		vector<int>	getHomeProvinces() const;

		string		getName()					const { return name; };
		double		getMovementProgress()	const { return movementProgress; };
		vector<int>	getPath()					const { return path; };
		int			getLocation()				const { return location; };

		double	getCurrentInfantryPSE()	const { return currentInfantryPSE; };
		double	getMaxInfantryPSE()		const { return maxInfantryPSE; };
		double	getCurrentCavalryPSE()	const { return currentCavalryPSE; };
		double	getMaxCavalryPSE()		const { return maxCavalryPSE; };
		double	getCurrentShips()			const { return currentShips; };
		double	getMaxShips()				const { return maxShips; };
	private:
		string					name;
		vector<CK2Subunit*>	subunits;
		double					movementProgress;
		vector<int>				path;
		int						location;

		double	currentInfantryPSE;
		double	maxInfantryPSE;
		double	currentCavalryPSE;
		double	maxCavalryPSE;
		double	currentShips;
		double	maxShips;
};




#endif	// CK2ARMY_H_