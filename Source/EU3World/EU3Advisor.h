#ifndef EU3Advisor_H_
#define EU3Advisor_H_


#include <string>
#include "..\Date.h"
#include "..\Mappers.h"
using namespace std;


class Object;
class CK2Character;
class EU3Province;

class EU3Advisor
{
	public:
		EU3Advisor(CK2Character*, inverseProvinceMapping, map<int, EU3Province*>, date);
		void	outputInProvince(FILE*);
		void	outputInActive(FILE*);
		void	setLocation(int);
		date	getDate();
	private:
		string	name;
		int		id;
		string	dynasty;

		string	advisorType;
		int		advisorSkill;
		int		location;
		string	home;

		date		startDate;
};



#endif // EU3Advisor_H_