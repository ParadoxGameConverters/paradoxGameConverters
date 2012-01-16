#include "Date.h"

date::date(string _init)
{
	if (_init.length() < 1)
		return;

	if (_init[0] == '\"')
		_init = _init.substr(1, _init.length() - 2);
	int first_dot = _init.find_first_of('.');
	int last_dot = _init.find_last_of('.');
	year	= atoi( _init.substr(0, first_dot).c_str() );
	month	= atoi( _init.substr(first_dot + 1, last_dot - first_dot).c_str() );
	day		= atoi( _init.substr(last_dot + 1, 2).c_str() );
}

date::date(const date& _init)
{
	year = _init.year;
	month = _init.month;
	day = _init.day;
}

date& date::operator=(const date& _rhs)
{
	year = _rhs.year;
	month = _rhs.month;
	day = _rhs.day;
	return *this;
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