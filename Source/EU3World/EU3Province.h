#include <string>
#include "..\Parsers\Object.h"
#include "..\Date.h"
using namespace std;


class EU3Province
{
	public:
		void init(int num, Object* obj, date startDate);
		void output(FILE*);
	private:
		int		num;
		string	owner;
};