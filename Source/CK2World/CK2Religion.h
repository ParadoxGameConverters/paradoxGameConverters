#ifndef CK2RELIGION_H_
#define CK2RELIGION_H_

#include <string>
#include <map>
using namespace std;

class Object;

class CK2Religion
{
public:
	CK2Religion(Object*, string group);

	// exactly one of these four functions should return true for any given pairing
	bool isSameReligion(const CK2Religion* other) const;
	bool isHereticTo(const CK2Religion* other) const;
	bool isRelatedTo(const CK2Religion* other) const;
	bool isInfidelTo(const CK2Religion* other) const;

	string getName() const { return name; }
	string getGroup() const { return group; }

	static void parseReligions(Object* obj);
	static CK2Religion* getReligion(string name);

private:
	string name;
	string parent;
	string group;

	static map<string, CK2Religion*> all_religions;
};

#endif // CK2RELIGION_H_