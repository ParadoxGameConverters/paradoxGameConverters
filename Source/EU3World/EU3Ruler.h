#ifndef EU3RULER_H_
#define EU3RULER_H_


#include <string>
#include "..\Parsers\Object.h"
#include "..\Date.h"
#include "..\CK2World\CK2Character.h"
using namespace std;


class EU3Ruler
{
	public:
		EU3Ruler(string name, int dip, int adm, int mil, string dynasty);
		EU3Ruler(CK2Character*);
		EU3Ruler(Object*);
		void	output(FILE*);
		int	getID();
		date	getBirthDate();
	private:
		string	name;
		int		diplomacy;
		int		administration;
		int		military;
		int		id;
		string	dynasty;
		date		birthDate;
};



#endif // EU3RULER_H_