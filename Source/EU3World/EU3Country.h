#ifndef EU3COUNTRY_H_
#define EU3COUNTRY_H_



#include <string>
#include <vector>
using namespace std;


class EU3Country
{
	public:
		void		output(FILE*);
		void		init(string tag, string countryFile);
		string	getTag();
	private:
		string	tag;
		string	countryFile;
};



#endif	// EU3COUNTRY_H_