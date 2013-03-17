#ifndef CK2ARMY_H_
#define CK2ARMY_H_


#include "..\Parsers\Object.h"
#include <string>
using namespace std;


class CK2Subunit
{
	public:
		CK2Subunit(const Object* obj);
		double	getCurrentInfantryPSE()	const { return currentInfantryPSE; };
		double	getMaxInfantryPSE()		const { return maxInfantryPSE; };
		double	getCurrentCavalryPSE()	const { return currentCavalryPSE; };
		double	getMaxCavalryPSE()		const { return maxCavalryPSE; };
	private:
		double	currentInfantryPSE;
		double	maxInfantryPSE;
		double	currentCavalryPSE;
		double	maxCavalryPSE;
};


class CK2Army
{
	public:
		CK2Army(const Object* obj);

		string					getName()					const { return name; };
		double					getMovementProgress()	const { return movementProgress; };
		vector<int>				getPath()					const { return path; };
		int						getLocation()				const { return location; };

		double	getCurrentInfantryPSE()	const { return currentInfantryPSE; };
		double	getMaxInfantryPSE()		const { return maxInfantryPSE; };
		double	getCurrentCavalryPSE()	const { return currentCavalryPSE; };
		double	getMaxCavalryPSE()		const { return maxCavalryPSE; };
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
};




#endif	// CK2ARMY_H_