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



#ifndef HoI4LEADER_H_
#define HoI4LEADER_H_


#include <stdio.h>
#include <string>
#include <vector>
#include "../Mappers/Mapper.h"
#include "../V2World/V2Leader.h"
using namespace std;


class HoI4Leader
{
	public:
		HoI4Leader(vector<string>& firstNames, vector<string>& lastNames, const string& _country, const string& _type, leaderTraitsMap& _traitsMap, vector<string>& portraits);
		HoI4Leader(V2Leader* srcLeader, const string& _country, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap, vector<string>& portraits);
		void output(FILE* output);

		string getType()	const { return type; }

	private:
		HoI4Leader(const HoI4Leader&) = delete;
		HoI4Leader& operator=(const HoI4Leader&) = delete;

		unsigned int		ID;
		string				name;
		string				country;
		string				type;
		int					skill;
		int					rank;
		string				picture;
		vector<string>	traits;
};



#endif	// HoI4LEADER_H_
