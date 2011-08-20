#ifndef EU3PROVINCE_H_
#define EU3PROVINCE_H_


#include "Parsers/Object.h"
#include "Date.h"


struct EU3PopRatio {
	string culture;
	string religion;
	double popRatio;
};

class EU3Province {
	public:
		void				init(Object* obj);
		int				getNum();
		void				setOwner(string);
		string			getOwner();
		vector<string>	getCores();
		void				addCore(string);
		void				removeCore(string);
		string			getCulture();
		string			getReligion();
		int				getPopulation();
		bool				isColony();
		date				getLastPossessedDate(string Tag);
		bool				wasColonised();
		void				buildPopRatios();
		vector<EU3PopRatio>	getPopRatios();
	private:
		void							decayPopRatios(date olddate, date newdate, EU3PopRatio& currentPop);
		int									num;
		string								owner;
		//controller
		vector<string>						cores;
		string								culture;
		string								religion;
		int									population;
		bool									colony;
		map<string, date>				lastPossessedDate;
		vector< pair<date, string> >	ownershipHistory;
		vector< pair<date, string> >	religionHistory;
		vector< pair<date, string> >	cultureHistory;
		vector<EU3PopRatio>				popRatios;
};


#endif // EU3PROVINCE_H_