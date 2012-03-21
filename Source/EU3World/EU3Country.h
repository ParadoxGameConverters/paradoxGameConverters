#ifndef EU3COUNTRY_H_
#define EU3COUNTRY_H_



#include <string>
#include <vector>
#include "..\CK2World\CK2Title.h"
using namespace std;


class EU3Country
{
	public:
		void		output(FILE*);
		void		init(string tag, string countryFile);
		void		convert(CK2Title*);
		string	getTag();
	private:
		string	tag;
		string	historyFile;
		string	government;
};



#endif	// EU3COUNTRY_H_