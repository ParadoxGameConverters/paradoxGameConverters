/*Copyright (c) 2017 The Paradox Game Converters Project

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
#include <vector>
#include "Object.h"
#include "OSCompatibilityLayer.h"
using namespace std;



date::date(string _init):
	year(1),
	month(1),
	day(1)
{
	if (_init.length() < 1)
	{
		return;
	}

	if (_init[0] == '\"')
	{
		_init = _init.substr(1, _init.length() - 2);
	}
	const int first_dot	= _init.find_first_of('.');	// the position of the first period in the date
	const int last_dot	= _init.find_last_of('.');		// the position of the second period in the date

	try
	{
		year						= stoi( _init.substr(0, first_dot) );
		month						= stoi( _init.substr(first_dot + 1, last_dot - first_dot) );
		day						= stoi( _init.substr(last_dot + 1, 2) );
	}
	catch (const std::exception& e)
	{
		LOG(LogLevel::Warning) << "Problem inputting date: " << e.what();
		year = 0;
		month = 0;
		day = 0;
	}
}


date::date(const date& _init)
{
	year = _init.year;
	month = _init.month;
	day = _init.day;
}


const date& date::operator=(const date& _rhs)
{
	year = _rhs.year;
	month = _rhs.month;
	day = _rhs.day;
	return *this;
}


date::date(const shared_ptr<Object> _init)
{
	auto dateSubObj = _init->safeGetObject("year");	// the date within the larger object
	if (dateSubObj)
	{
		// date specified by year=, month=, day=
		year	= _init->safeGetInt("year");
		month	= _init->safeGetInt("month");
		day	= _init->safeGetInt("day");
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


float date::diffInYears(const date& _rhs) const
{
	float years = static_cast<float>(year - _rhs.year);	// the difference in years
	years += (calculateDayInYear() - _rhs.calculateDayInYear()) / 365;

	return years;
}


const int daysByMonth[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
int date::calculateDayInYear() const
{
	return day + daysByMonth[month - 1];
}


void date::delayedByMonths(const int _months)
{
	year += _months / 12;
	month += _months % 12;
	if (month > 12)
	{
		year++;
		month -= 12;
	}
	return;
}


void date::subtractYears(const int _years)
{
	year -= _years;
}


bool date::isSet() const
{
	const date default_date;	// an instance with the default date
	return (*this != default_date);
}


string date::toString() const
{
	stringstream buf;
	buf << year << '.' << month << '.' << day;
	return buf.str();
}