#ifndef DATE_H_
#define DATE_H_

#include <string>
using namespace std;



class Object;

struct date
{
	date() : year(1), month(1), day(1) {};
	date(string _init);
	date(const date& _init);
	date(const Object* _init);

	date& operator=(const date& _rhs);
	bool operator==(const date& _rhs) const;
	bool operator!=(const date& _rhs) const;
	bool operator<(const date& _rhs) const;
	bool operator>(const date& _rhs) const;
	bool operator<=(const date& _rhs) const;
	bool operator>=(const date& _rhs) const;
	
	float diffInYears(const date& _rhs) const;

	bool isSet() const;
	string toString() const;

	int year;
	int month;
	int day;
};

#endif // _DATE_H