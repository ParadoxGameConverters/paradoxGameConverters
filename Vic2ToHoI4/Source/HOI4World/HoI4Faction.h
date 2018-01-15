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



#ifndef HOI4_FACTION_H
#define HOI4_FACTION_H



#include <vector>
using namespace std;



class HoI4Country;



class HoI4Faction
{

	public:
		HoI4Faction(shared_ptr<HoI4Country> leader, const vector<shared_ptr<HoI4Country>>& members): Factionleader(leader), Factionmembers(members) {}

		shared_ptr<HoI4Country> getLeader() const { return Factionleader; }
		vector<shared_ptr<HoI4Country>> getMembers() const { return Factionmembers; }
		void addMember(shared_ptr<HoI4Country> addedCon) { Factionmembers.push_back(addedCon); }

	private:
		HoI4Faction(const HoI4Faction&) = delete;
		HoI4Faction& operator=(const HoI4Faction&) = delete;

		shared_ptr<HoI4Country> Factionleader;
		vector<shared_ptr<HoI4Country>> Factionmembers;

};



#endif // HOI4_FACTION_H