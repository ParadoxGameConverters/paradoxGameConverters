/*Copyright (c) 2018 The Paradox Game Converters Project

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
#include <array>
#include <vector>
#include "OSCompatibilityLayer.h"



date::date(std::string _init):
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


bool date::operator==(const date& _rhs) const noexcept
{
	return ((year == _rhs.year)
		 && (month == _rhs.month)
		 && (day == _rhs.day));
}


bool date::operator!=(const date& _rhs) const noexcept
{
	return !(*this == _rhs);
}


bool date::operator<(const date& _rhs) const noexcept
{
	return ((year < _rhs.year)
		|| ((year == _rhs.year) && (month < _rhs.month))
		|| ((year == _rhs.year) && (month == _rhs.month) && (day < _rhs.day)));
}


bool date::operator>(const date& _rhs) const noexcept
{
	return ((year > _rhs.year)
		|| ((year == _rhs.year) && (month > _rhs.month))
		|| ((year == _rhs.year) && (month == _rhs.month) && (day > _rhs.day)));
}


bool date::operator<=(const date& _rhs) const noexcept
{
	return ((*this == _rhs) || (*this < _rhs));
}


bool date::operator>=(const date& _rhs) const noexcept
{ 
	return ((*this == _rhs) || (*this > _rhs));
}


std::ostream& operator<<(std::ostream& out, const date& d)
{
	out << d.year << '.' << d.month << '.' << d.day;
	return out;
}


float date::diffInYears(const date& _rhs) const noexcept
{
	float years = static_cast<float>(year - _rhs.year);	// the difference in years
	years += static_cast<float>(calculateDayInYear() - _rhs.calculateDayInYear()) / 365;

	return years;
}


const std::array<int, 12> daysByMonth = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
int date::calculateDayInYear() const noexcept
{
	return day + daysByMonth[month - 1];
}


void date::increaseByMonths(const int _months) noexcept
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


void date::subtractYears(const int _years) noexcept
{
	year -= _years;
}


bool date::isSet() const noexcept
{
	const date default_date;	// an instance with the default date
	return (*this != default_date);
}


std::string date::toString() const
{
	std::stringstream buf;
	buf << year << '.' << month << '.' << day;
	return buf.str();
}