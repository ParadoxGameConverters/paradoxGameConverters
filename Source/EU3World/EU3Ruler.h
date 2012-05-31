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
		EU3Ruler();
		EU3Ruler(CK2Character*);
		EU3Ruler(Object*);
		string	getName();
		void		setRegnalNum(int);
		int		getRegnalNum();
		void		outputAsMonarch(FILE*);
		void		outputAsHeir(FILE*);
		int		getID();
		date		getBirthDate();
		void		setMonarchName();
	private:
		void outputRegnalNum(FILE*);

		string	name;
		int		regnalNum;
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