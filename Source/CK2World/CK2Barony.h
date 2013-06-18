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

		CK2Title*		getTitle()				const { return title; };
		CK2Province*	getProvince()			const { return province; };
		string			getType()				const { return type; };
		double			getBaseTaxProxy()		const { return baseTaxProxy; };
		double			getPopProxy()			const { return popProxy; };
		double			getManpowerProxy()	const { return manpowerProxy; };
		double			getTechBonus()			const { return techBonus; };
		double			getPSE()					const { return PSE; };
		double			getFreeTroops()		const { return freeTroops; };
		double			getSerfTroops()		const	{ return serfTroops; };
		int				getShips()				const { return ships; };
		double			getFortLevel()			const { return fortLevel; };
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

		double	PSE;
		int		ships;
		double	freeTroops;
		double	serfTroops;
};



#endif // CK2BARONY_H_