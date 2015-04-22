/*Copyright (c) 2015 The Paradox Game Converters Project

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



#ifndef HOI3MINISTER_H_
#define HOI3MINISTER_H_


#include <stdio.h>
#include <string>
#include <vector>
#include "../Mapper.h"
using namespace std;


class HoI3Minister
{
	public:
		HoI3Minister(vector<string>& firstNames, vector<string>& lastNames, string _ideology, governmentJob job, governmentJobsMap& jobMap, vector<string>& portraits);
		void output(FILE* output);

		string			getFirstJob()	const { return roles[0].first; }
		unsigned int	getID()			const { return ID; }

	private:
		unsigned int	ID;
		string			name;
		string			ideology;
		double			loyalty;
		string			picture;
		vector<pair<string, string>>	roles;	
};



#endif	// HOI3MINISTER_H_