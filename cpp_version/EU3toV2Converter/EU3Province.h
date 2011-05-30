#ifndef EU3PROVINCE_H_
#define EU3PROVINCE_H_


#include "Parsers/Object.h"


typedef struct
{
	int year;
	int month;
	int day;
} date;

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
		bool				isColony();
		date				getLastPossessedDate(string Tag);
	private:
		int									num;
		string								owner;
		//controller
		vector<string>						cores;
		string								culture;
		string								religion;
		int									population;
		bool									colony;
		vector< pair<string, date> >	lastPossessedDate;
};


#endif // EU3PROVINCE_H_