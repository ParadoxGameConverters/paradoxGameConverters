#ifndef EU3PROVINCE_H_
#define EU3PROVINCE_H_


#include "Date.h"
#include <string>
#include <vector>
#include <map>
using namespace std;

class Object;
class EU3Country;



struct EU3PopRatio {
	string culture;
	string religion;
	double popRatio;
};


class EU3Province {
	public:
		EU3Province(Object* obj);

		void						addCore(string tag);
		void						removeCore(string tag);
		bool						wasColonised() const;
		bool						wasPaganConquest() const;
		bool						hasBuilding(string building) const;

		vector<EU3Country*>	getCores(const map<string, EU3Country*>& countries) const;
		date						getLastPossessedDate(string tag) const;

		int						getNum()				const { return num; };
		string					getOwnerString()	const { return ownerString; };
		EU3Country*				getOwner()			const { return owner; };
		int						getPopulation()	const { return population; };
		bool						isColony()			const { return colony; };
		bool						isCOT()				const { return centerOfTrade; };
		vector<EU3PopRatio>	getPopRatios()		const { return popRatios; };

		void						setOwner(EU3Country* newOwner)	{ owner = newOwner; };
		void						setCOT(bool isCOT)					{ centerOfTrade = isCOT; };
	private:
		void	checkBuilding(const Object* provinceObj, string building);
		void	buildPopRatios();
		void	decayPopRatios(date olddate, date newdate, EU3PopRatio& currentPop);

		int									num;
		string								ownerString;
		EU3Country*							owner;
		//controller
		vector<string>						cores;
		int									population;
		bool									colony;
		bool									centerOfTrade;
		vector< pair<date, string> >	ownershipHistory;
		map<string, date>					lastPossessedDate;
		vector< pair<date, string> >	religionHistory;
		vector< pair<date, string> >	cultureHistory;
		vector<EU3PopRatio>				popRatios;
		map<string, bool>					buildings;
};


#endif // EU3PROVINCE_H_