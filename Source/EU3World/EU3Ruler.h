#ifndef EU3RULER_H_
#define EU3RULER_H_


#include <string>
#include "..\Parsers\Object.h"
using namespace std;


class EU3Ruler
{
	public:
		EU3Ruler(string name, int dip, int adm, int mil, string dynasty);
		EU3Ruler(Object*);
		void output(FILE*);
		int	getID();
	private:
		string	name;
		int		diplomacy;
		int		administration;
		int		military;
		int		id;
		string	dynasty;
};



#endif // EU3RULER_H_