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



#ifndef DATE_H_
#define DATE_H_



#include <iostream>
#include <memory>
#include <string>



struct date
{
	public:
		explicit date(std::string _init);

		date() = default; 
		date(const date&) = default;
		date(date&&) = default;
		~date() = default;
		date& operator=(const date&) = default;

		bool operator==(const date& _rhs) const noexcept;
		bool operator!=(const date& _rhs) const noexcept;
		bool operator<(const date& _rhs) const noexcept;
		bool operator>(const date& _rhs) const noexcept;
		bool operator<=(const date& _rhs) const noexcept;
		bool operator>=(const date& _rhs) const noexcept;

		friend std::ostream& operator<<(std::ostream&, const date&);
	
		float diffInYears(const date& _rhs) const noexcept;

		void increaseByMonths(const int _months) noexcept;
		void subtractYears(const int _years) noexcept;

		bool isSet() const noexcept;
		std::string toString() const;

	private:
		int calculateDayInYear() const noexcept;

		int year = 1;
		int month = 1;
		int day = 1;
};



#endif // _DATE_H