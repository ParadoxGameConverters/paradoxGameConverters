/*Copyright (c) 2014 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/


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


ostream& operator<<(ostream& out, const date& d)
{
	out << d.year << '.' << d.month << '.' << d.day;
	return out;
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