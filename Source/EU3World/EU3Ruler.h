#ifndef EU3RULER_H_
#define EU3RULER_H_


#include <string>
#include "..\Date.h"
using namespace std;


class Object;
class CK2Character;

class EU3Ruler
{
	public:
		EU3Ruler(string name, int dip, int adm, int mil, string dynasty);
		EU3Ruler(CK2Character*);
		EU3Ruler(Object*);
		void	outputAsMonarch(FILE*);
		void	outputAsHeir(FILE*);
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
		date		deathDate;
		int		claim;
		string	monarchName;
		bool		female;
};



#endif // EU3RULER_H_