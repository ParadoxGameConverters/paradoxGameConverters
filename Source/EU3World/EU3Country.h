#ifndef EU3COUNTRY_H_
#define EU3COUNTRY_H_



#include <string>
#include <vector>
#include "EU3Ruler.h"
#include "EU3History.h"
#include "..\CK2World\CK2Title.h"
#include "..\Date.h"
using namespace std;


class EU3Country
{
	public:
		void		output(FILE*);
		void		init(string tag, string countryFile, date startDate);
		void		convert(CK2Title*);
		string	getTag();
	private:
		string					tag;
		string					historyFile;
		string					government;
		EU3Ruler*				monarch;
		vector<EU3History*>	history;
		vector<EU3Ruler*>		previousMonarchs;
};



#endif	// EU3COUNTRY_H_