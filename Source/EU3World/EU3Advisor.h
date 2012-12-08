#ifndef EU3Advisor_H_
#define EU3Advisor_H_


#include <string>
#include "..\Date.h"
#include "..\Mappers.h"
using namespace std;


class Object;
class CK2Character;
class EU3Province;
class EU3Country;

class EU3Advisor
{
	public:
		EU3Advisor(Object*, const map<int, EU3Province*>&);
		EU3Advisor(CK2Character*, const inverseProvinceMapping&, const map<int, EU3Province*>&, date);

		void	outputInProvince(FILE*);
		void	outputInActive(FILE*);

		void	setLocation(int _location)	{ location = _location; };
		void	setHome(EU3Country* _home)	{ home = _home; };

		int	getLocation()	const { return location; };
		date	getStartDate()	const { return startDate; };
		date	getDeathDate()	const { return deathDate; };

	private:
		string		name;
		int			id;
		string		dynasty;

		string		advisorType;
		int			advisorSkill;
		int			location;
		EU3Country*	home;

		date			startDate;
		date			deathDate;
};



#endif // EU3Advisor_H_