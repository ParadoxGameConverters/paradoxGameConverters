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



#ifndef CK2BARONY_H_
#define CK2BARONY_H_



#include "..\Mappers.h"
#include <string>
using namespace std;


class Object;
class CK2Building;
class CK2BuildingFactory;
class CK2Title;
class CK2Province;

class CK2Barony
{
	public:
		CK2Barony(Object*, CK2Title*, CK2Province*, const CK2BuildingFactory*);

		CK2Title*		getTitle()					const { return title; };
		CK2Province*	getProvince()				const { return province; };
		string			getType()					const { return type; };
		double			getBaseTaxProxy()			const { return baseTaxProxy; };
		double			getPopProxy()				const { return popProxy; };
		double			getManpowerProxy()		const { return manpowerProxy; };
		double			getTechBonus()				const { return techBonus; };
		double			getPSE()						const { return PSE; };
		double			getFreeTroops()			const { return freeTroops; };
		double			getSerfTroops()			const	{ return serfTroops; };
		int				getShips()					const { return ships; };
		int				getMaxShips()				const { return maxShips; };
		double			getFortLevel()				const { return fortLevel; };
		int				getQualityBuildings()	const { return qualityBuildings; };
	private:
		void	determineBaseTaxProxy();
		void	determinePopProxy();
		void	determineManpowerProxy();
		void	determineTechBonus();
		void	determineFortLevel();

		CK2Title*		title;
		CK2Province*	province;

		string							type;
		vector<const CK2Building*>	buildings;
		int								proxyMultiplier;
		double							baseTaxProxy;
		double							popProxy;
		double							manpowerProxy;
		double							techBonus;
		double							fortLevel;
		int								qualityBuildings;

		double	PSE;
		int		ships;
		int		maxShips;
		double	freeTroops;
		double	serfTroops;
};



#endif // CK2BARONY_H_