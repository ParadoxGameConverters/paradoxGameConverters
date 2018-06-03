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



#include "Event.h"
#include "../V2World/Party.h"
#include "newParser.h"
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>



class HoI4Country;



namespace HoI4
{

	class OnActions;

class Events: commonItems::parser
{
	public:
		Events() = default;

		void output() const;
		void createFactionEvents(std::shared_ptr<HoI4Country> Leader, std::shared_ptr<HoI4Country> newAlly);
		void createAnnexEvent(std::shared_ptr<HoI4Country> Annexer, std::shared_ptr<HoI4Country> Annexed);
		void createSudetenEvent(std::shared_ptr<HoI4Country> Annexer, std::shared_ptr<HoI4Country> Annexed, const std::vector<int>& claimedStates);
		void createTradeEvent(std::shared_ptr<HoI4Country> leader, std::shared_ptr<HoI4Country> GC);
		void createPoliticalEvents(const std::set<std::string>& majorIdeologies);
		void createWarJustificationEvents(const std::set<std::string>& majorIdeologies);
		void importElectionEvents(const std::set<std::string>& majorIdeologies, HoI4::OnActions& onActions);
		void addPartyChoiceEvent(const std::string& countryTag, const std::set<Vic2::Party, std::function<bool (const Vic2::Party&, const Vic2::Party&)>>& parties, HoI4::OnActions& onActions, const std::set<std::string>& majorIdeologies);
		void createStabilityEvents(const std::set<std::string>& majorIdeologies);

		int getCurrentNationFocusEventNum() const { return nationalFocusEventNumber; }


	private:
		Events(const Events&) = delete;
		Events& operator=(const Events&) = delete;

		void outputNationalFocusEvents() const;
		void outputNewsEvents() const;
		void outputPoliticalEvents() const;
		void outputWarJustificationEvents() const;
		void outputElectionEvents() const;
		void outputStabilityEvents() const;

		void addMinisterRevolutionEvents(const std::set<std::string>& majorIdeologies);
		void addDemocraticMinisterRevolutionEvents(const std::set<std::string>& majorIdeologies);
		void addFiftyPercentEvents(const std::set<std::string>& majorIdeologies);
		void addRevolutionEvents(const std::set<std::string>& majorIdeologies);
		void addSuppressedEvents(const std::set<std::string>& majorIdeologies);
		std::string getIdeologicalPicture(const std::string& ideology) const;

		std::vector<Event> newsEvents;
		int newsEventNumber = 237;
		std::vector<Event> nationalFocusEvents;
		int nationalFocusEventNumber = 0;
		std::vector<Event> politicalEvents;
		int politicalEventNumber = 1;
		std::vector<Event> warJustificationEvents;
		std::vector<Event> electionEvents;
		int electionEventNumber = 4;
		std::map<std::string, Event> stabilityEvents;
		std::map<std::string, Event> strikesEvents;
		std::map<std::string, Event> mutinyEvents;
};

}



#endif // HOI4_EVENTS_H