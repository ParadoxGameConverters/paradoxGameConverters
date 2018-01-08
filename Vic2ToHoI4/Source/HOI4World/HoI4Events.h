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



#ifndef HOI4_EVENTS_H
#define HOI4_EVENTS_H



#include "HoI4Event.h"
#include "../V2World/V2Party.h"
#include <functional>
#include <memory>
#include <set>
#include <string>
using namespace std;



class HoI4Country;
class HoI4OnActions;



class HoI4Events
{
	public:
		HoI4Events();

		void output() const;
		void createFactionEvents(shared_ptr<HoI4Country> Leader, shared_ptr<HoI4Country> newAlly);
		void createAnnexEvent(shared_ptr<HoI4Country> Annexer, shared_ptr<HoI4Country> Annexed);
		void createSudetenEvent(shared_ptr<HoI4Country> Annexer, shared_ptr<HoI4Country> Annexed, const vector<int>& claimedStates);
		void createTradeEvent(shared_ptr<HoI4Country> leader, shared_ptr<HoI4Country> GC);
		void createPoliticalEvents(const set<string>& majorIdeologies);
		void createWarJustificationEvents(const set<string>& majorIdeologies);
		void createElectionEvents(const set<string>& majorIdeologies, HoI4OnActions* onActions);
		void addPartyChoiceEvent(const string& countryTag, const set<V2Party, function<bool (const V2Party&, const V2Party&)>>& parties, HoI4OnActions* onActions, const set<string>& majorIdeologies);

		int getCurrentNationFocusEventNum() const { return nationalFocusEventNumber; }


	private:
		HoI4Events(const HoI4Events&) = delete;
		HoI4Events& operator=(const HoI4Events&) = delete;

		void outputNationalFocusEvents() const;
		void outputNewsEvents() const;
		void outputPoliticalEvents() const;
		void outputWarJustificationEvents() const;
		void outputElectionEvents() const;

		void addMinisterRevolutionEvents(const set<string>& majorIdeologies);
		void addDemocraticMinisterRevolutionEvents(const set<string>& majorIdeologies);
		void addFiftyPercentEvents(const set<string>& majorIdeologies);
		void addRevolutionEvents(const set<string>& majorIdeologies);
		void addSuppressedEvents(const set<string>& majorIdeologies);
		string getIdeologicalPicture(const string& ideology) const;

		void addIdeologyInGovernmentEvents(const set<string>& majorIdeologies, HoI4OnActions* onActions);
		void addIdeologyInfluenceForeignPolicyEvents(const set<string>& majorIdeologies);
		void addDemocraticPartiesInMinorityEvent(const set<string>& majorIdeologies, HoI4OnActions* onActions);
		void addIdeologicalMajorityEvent(const set<string>& majorIdeologies, HoI4OnActions* onActions);
		void addWartimeExceptionEvent(const set<string>& majorIdeologies, HoI4OnActions* onActions);
		void addGovernmentContestedEvent(const set<string>& majorIdeologies, HoI4OnActions* onActions);

		vector<HoI4Event> newsEvents;
		int newsEventNumber;
		vector<HoI4Event> nationalFocusEvents;
		int nationalFocusEventNumber;
		vector<HoI4Event> politicalEvents;
		int politicalEventNumber;
		vector<HoI4Event> warJustificationEvents;
		vector<HoI4Event> electionEvents;
		int electionEventNumber;
};



#endif // HOI4_EVENTS_H