#ifndef CK2BARONY_H_
#define CK2BARONY_H_



#include <string>
#include <map>
using namespace std;


class Object;
class CK2Title;
class CK2Province;

class CK2Barony
{
	public:
		CK2Barony(Object*, CK2Title*, CK2Province*);

		CK2Title*		getTitle()		const { return title; };
		CK2Province*	getProvince()	const { return province; };

		float				getPopProxy() const;
	private:
		CK2Title*		title;
		CK2Province*	province;

		string				type;
		map<string, bool>	buildings;
};



#endif // CK2BARONY_H_