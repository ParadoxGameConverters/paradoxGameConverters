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



#ifndef _HOI4_STRATEGIC_REGION
#define _HOI4_STRATEGIC_REGION



#include "newParser.h"
#include <string>
#include <vector>



class HoI4StrategicRegion: commonItems::parser
{
	public:
		explicit HoI4StrategicRegion(const std::string& _filename);
		void output(const std::string& path) const;

		int getID() const { return ID; }
		std::vector<int>	getOldProvinces() const	{ return oldProvinces; }

		void addNewProvince(int province) { newProvinces.push_back(province); }

	private:
		HoI4StrategicRegion(const HoI4StrategicRegion&) = delete;
		HoI4StrategicRegion& operator=(const HoI4StrategicRegion&) = delete;

		std::string filename;
		int ID = 0;
		std::vector<int> oldProvinces;
		std::vector<int> newProvinces;
		std::string weather;
};



#endif // _HOI4_STRATEGIC_REGION
