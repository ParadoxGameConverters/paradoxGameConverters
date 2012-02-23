#include <string>
#include "..\Parsers\Object.h"
using namespace std;


class EU3Province
{
	public:
		void init(int num, Object*);
		void output(FILE*);
	private:
		int		num;
		string	owner;
};