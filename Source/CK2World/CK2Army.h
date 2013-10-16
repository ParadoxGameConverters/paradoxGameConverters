/*Copyright (c) 2013 The CK2 to EU3 Converter Project
 
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



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

		vector<CK2Army*>	getTransportedArmies()	const { return transportedArmies; };
		double				getCurrentInfantryPSE()	const { return currentInfantryPSE; };
		double				getMaxInfantryPSE()		const { return maxInfantryPSE; };
		double				getCurrentCavalryPSE()	const { return currentCavalryPSE; };
		double				getMaxCavalryPSE()		const { return maxCavalryPSE; };
		double				getCurrentShips()			const { return currentShips; };
		double				getMaxShips()				const { return maxShips; };
	private:
		string					name;
		vector<CK2Subunit*>	subunits;
		double					movementProgress;
		vector<int>				path;
		int						location;

		vector<CK2Army*>		transportedArmies;

		double	currentInfantryPSE;
		double	maxInfantryPSE;
		double	currentCavalryPSE;
		double	maxCavalryPSE;
		double	currentShips;
		double	maxShips;
};




#endif	// CK2ARMY_H_