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

		void						addCore(string);
		void						removeCore(string);
		bool						wasColonised();
		bool						wasPaganConquest(string ownerReligion);
		bool						hasBuilding(string building);

		int						getNum();
		string					getOwner();
		vector<EU3Country*>	getCores(map<string, EU3Country*>);
		int						getPopulation();
		bool						isColony();
		bool						isCOT();
		date						getLastPossessedDate(string Tag);
		vector<EU3PopRatio>	getPopRatios();

		void						setOwner(string);
		void						setCOT(bool isCOT);

	private:
		void	checkBuilding(Object* provinceObj, string building);
		void	buildPopRatios();
		void	decayPopRatios(date olddate, date newdate, EU3PopRatio& currentPop);

		int									num;
		string								owner;
		//controller
		vector<string>						cores;
		//string								religion;
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