#include "Date.h"
#include "Parsers\Object.h"



date::date(const string _init)
{
	if (_init.length() < 1)
	{
		return;
	}

	string subStr;
	if (_init[0] == '\"')
	{
		subStr = _init.substr(1, _init.length() - 2);
	}
	else
	{
		subStr = _init;
	}
	int first_dot	= subStr.find_first_of('.');
	int last_dot	= subStr.find_last_of('.');
	year				= atoi( subStr.substr(0, first_dot).c_str() );
	month				= atoi( subStr.substr(first_dot + 1, last_dot - first_dot).c_str() );
	day				= atoi( subStr.substr(last_dot + 1, 2).c_str() );
}


date::date(const date& _init)
{
	year	= _init.year;
	month	= _init.month;
	day	= _init.day;
}


date& date::operator=(const date& _rhs)
{
	year	= _rhs.year;
	month	= _rhs.month;
	day	= _rhs.day;
	return *this;
}


date::date(const Object* _init)
{
	vector<Object*> dateSubObj = _init->getValue("year");
	if (dateSubObj.size() > 0)
	{
		// date specified by year=, month=, day=
		year	= atoi(_init->getLeaf("year").c_str());
		month	= atoi(_init->getLeaf("month").c_str());
		day	= atoi(_init->getLeaf("day").c_str());
	}
	else
	{
		// date specified by year.month.day
		// build another date object via date(string&) and copy it to this one
		(*this) = date(_init->getLeaf());
	}
}


bool date::operator==(const date& _rhs) const
{
	return ((year == _rhs.year)
		 && (month == _rhs.month)
		 && (day == _rhs.day));
}


bool date::operator!=(const date& _rhs) const
{
	return !(*this == _rhs);
}


bool date::operator<(const date& _rhs) const
{
	return ((year < _rhs.year)
		|| ((year == _rhs.year) && (month < _rhs.month))
		|| ((year == _rhs.year) && (month == _rhs.month) && (day < _rhs.day)));
}


bool date::operator>(const date& _rhs) const
{
	return ((year > _rhs.year)
		|| ((year == _rhs.year) && (month > _rhs.month))
		|| ((year == _rhs.year) && (month == _rhs.month) && (day > _rhs.day)));
}


bool date::operator<=(const date& _rhs) const
{
	return ((*this == _rhs) || (*this < _rhs));
}


bool date::operator>=(const date& _rhs) const
{ 
	return ((*this == _rhs) || (*this > _rhs));
}


bool date::isSet() const
{
	date default_date;
	return (*this != default_date);
}


string date::toString() const
{
	char buf[16];
	sprintf_s(buf, 16, "%d.%d.%d", year, month, day);
	return string(buf);
}