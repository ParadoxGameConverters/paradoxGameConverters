#ifndef EU3PROVINCE_H_
#define EU3PROVINCE_H_


#include <string>
#include "..\Parsers\Object.h"
#include "..\Date.h"
using namespace std;


class EU3Province
{
	public:
		void init(int num, Object* obj, date startDate);
		void output(FILE*);
		void setNumber(int);
		void addCore(string);
		void setOwner(string);
		void setInHRE(bool);
	private:
		int				num;
		string			owner;
		vector<string>	cores;
		bool				inHRE;
};



#endif	// EU3PROVINCE_H_