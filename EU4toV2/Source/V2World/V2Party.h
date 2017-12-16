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



#include "Object.h"
#include "Date.h"
#include <memory>
using namespace std;



struct V2Party
{
	public:
		V2Party(shared_ptr<Object> party_obj);
		V2Party() : name("NULL PARTY") {};
		// Populates all data with default values satisfying the required ideology.
		V2Party(const std::string& name, const std::string& ideology);

		string name;
		string ideology;

		date start_date;
		date end_date;
	
		string economic_policy;
		string trade_policy;
		string religious_policy;
		string citizenship_policy;
		string war_policy;

		bool isActiveOn(date query) { return ((query > start_date) && (query < end_date)); };
};
