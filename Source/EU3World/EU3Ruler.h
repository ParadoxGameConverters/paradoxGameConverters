#ifndef EU3RULER_H_
#define EU3RULER_H_


#include <string>
#include <vector>
#include "..\Date.h"
using namespace std;


class Object;
class CK2Character;

class EU3Ruler
{
	public:
		EU3Ruler(Object* obj);
		EU3Ruler(CK2Character* src);

		void		outputAsMonarch(FILE*);
		void		outputAsRegent(FILE*);
		void		outputAsHeir(FILE*);

		void		setRegnalNum(vector<EU3Ruler*> previousMonarchs);
		void		setRegnalNum(int num)	{ regnalNum = num; };

		string	getName()			const { return name; };
		int		getRegnalNum()		const { return regnalNum; };
		int		getID()				const { return id; };
		
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