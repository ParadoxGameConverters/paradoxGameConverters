#ifndef HOI3COUNTRY_H_
#define HOI3COUNTRY_H_

#include <string>
#include <vector>
#include <map>
#include "Date.h"

#include "HoI3Relations.h"
#include "HoI3Army.h"
//#include "HoI3Creditor.h"
//#include "HoI3Leader.h"
#include "HoI3Alignment.h"

class HoI3Country
{
	public:
		void					init(string tag, string countryFile);
		void					initFromHistory();
		string				getTag() const;
		void					setSourceCountryIndex(int);
		int						getSourceCountryIndex();
		void					setCapital(int);
		void					output(FILE*);
		void					addRelations(HoI3Relations);
		HoI3Relations*			getRelations(string);
		void					addArmy(HoI3Army);
		void					sortRelations(const vector<string>& order);
		void					setMoney(double);
		void					setLastBankrupt(date);
		void					addLoan(string creditor, double size, double interest);
		void					setDiploPoints(double);
		//XXX: void					addLeader(HoI3Leader);
		void					setTechnology(string tech, int level);
		void					addProvince(int index);	// note: takes index into provinces array, NOT province num
		vector<int>				getProvinces() const;	// note: returns indexes into provinces array, NOT province nums
		HoI3Alignment*			getAlignment();

	private:
		void outputTech(FILE*);

		string				tag;
		int					capital;
		int					sourceCountryIndex;
		string				countryFile;
		vector<HoI3Relations>	relations;
		vector<HoI3Army>	armies;
		double				money;
		date				lastBankrupt;
		//XXX: vector<HoI3Creditor>	creditors;
		double				diploPoints;
		//XXX: vector<HoI3Leader>	leaders;
		map<string, int>	technologies;
		vector<int>			provinces;
		HoI3Alignment		alignment;
};


#endif	// HoI3COUNTRY_H_
