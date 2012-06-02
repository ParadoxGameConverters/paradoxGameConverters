#ifndef EU3COUNTRY_H_
#define EU3COUNTRY_H_



#include <string>
#include <vector>
#include "..\Date.h"
#include "..\Mappers.h"
using namespace std;


class CK2Title;
class EU3Ruler;
class EU3History;
class CK2Province;

class EU3Country
{
	public:
		EU3Country();
		void		output(FILE*);
		void		init(string tag, string countryFile, date startDate);
		void		convert(CK2Title*);
		string	getTag();
		string	getTechGroup();
	private:
		string					tag;
		string					historyFile;
		string					government;
		EU3Ruler*				monarch;
		EU3Ruler*				heir;
		vector<EU3History*>	history;
		vector<EU3Ruler*>		previousMonarchs;
		string					techGroup;
};



#endif	// EU3COUNTRY_H_