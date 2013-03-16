#ifndef CK2ARMY_H_
#define CK2ARMY_H_


#include "..\Parsers\Object.h"
#include <string>
using namespace std;


class CK2Subunit
{
	public:
		CK2Subunit(const Object* obj);
	private:
};


class CK2Army
{
	public:
		CK2Army(const Object* obj);

		string					getName()					const { return name; };
		vector<CK2Subunit*>	getSubunits()				const { return subunits; };
		double					getMovementProgress()	const { return movementProgress; };
		vector<int>				getPath()					const { return path; };
		int						getLocation()				const { return location; };
	private:
		string					name;
		vector<CK2Subunit*>	subunits;
		double					movementProgress;
		vector<int>				path;
		int						location;
};




#endif	// CK2ARMY_H_