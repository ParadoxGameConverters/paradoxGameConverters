/*Copyright (c) 2016 The Paradox Game Converters Project

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

#ifndef GOVERNMENTMAPPER_H
#define GOVERNMENTMAPPER_H

#include "Object.h"
#include <map>
#include <string>
using namespace std;



class V2Country;


struct govMapping
{
	wstring vic_gov;
	wstring HoI4_gov;
	wstring ruling_party_required;
	double require_political_reforms;
	double require_social_reforms_above;
	double require_social_reforms_below;
};


class governmentMapper
{
	public:
		governmentMapper();
		void initGovernmentMap(Object* obj);
		void initReforms(Object* obj);

		wstring	getGovernmentForCountry(const V2Country* country, const wstring _ideology);

		bool							areReformsInitialized() const	{ return reformsInitialized; }
		map<wstring, wstring>	getReformTypes() const			{ return reformTypes; }

		static governmentMapper* getInstance()
		{
			if (instance == NULL)
			{
				instance = new governmentMapper();
			}
			return instance;
		}

	private:
		static governmentMapper* instance;

		vector<govMapping>		governmentMap;
		map<wstring, wstring>	reformTypes;
		map<wstring, int>			politicalReformScores;
		map<wstring, int>			socialReformScores;
		int							totalPoliticalReforms;
		int							totalSocialReforms;
		bool							reformsInitialized;
};

#endif // GOVERNMENTMAPPER_H