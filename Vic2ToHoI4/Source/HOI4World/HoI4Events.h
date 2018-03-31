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
#include "newParser.h"
#include <functional>
#include <memory>
#include <set>
#include <string>



class HoI4Country;
class HoI4OnActions;



namespace HoI4
{
	class Events: commonItems::parser
	{
		public:
			Events();

			void output() const;
			void createFactionEvents(std::shared_ptr<HoI4Country> Leader, std::shared_ptr<HoI4Country> newAlly);
			void createAnnexEvent(std::shared_ptr<HoI4Country> Annexer, std::shared_ptr<HoI4Country> Annexed);
			void createSudetenEvent(std::shared_ptr<HoI4Country> Annexer, std::shared_ptr<HoI4Country> Annexed, const std::vector<int>& claimedStates);
			void createTradeEvent(std::shared_ptr<HoI4Country> leader, std::shared_ptr<HoI4Country> GC);
			void createPoliticalEvents(const std::set<std::string>& majorIdeologies);
			void createWarJustificationEvents(const std::set<std::string>& majorIdeologies);
			void addPartyChoiceEvent(const std::string& countryTag, const std::set<V2Party, std::function<bool (const V2Party&, const V2Party&)>>& parties, HoI4OnActions* onActions, const std::set<std::string>& majorIdeologies);

			int getCurrentNationFocusEventNum() const { return nationalFocusEventNumber; }


		private:
			Events(const Events&) = delete;
			Events& operator=(const Events&) = delete;

			void outputNationalFocusEvents() const;
			void outputNewsEvents() const;
			void outputPoliticalEvents() const;
			void outputWarJustificationEvents() const;
			void outputElectionEvents() const;

			void addMinisterRevolutionEvents(const std::set<std::string>& majorIdeologies);
			void addDemocraticMinisterRevolutionEvents(const std::set<std::string>& majorIdeologies);
			void addFiftyPercentEvents(const std::set<std::string>& majorIdeologies);
			void addRevolutionEvents(const std::set<std::string>& majorIdeologies);
			void addSuppressedEvents(const std::set<std::string>& majorIdeologies);
			std::string getIdeologicalPicture(const std::string& ideology) const;

			std::vector<Event> newsEvents;
			int newsEventNumber;
			std::vector<Event> nationalFocusEvents;
			int nationalFocusEventNumber;
			std::vector<Event> politicalEvents;
			int politicalEventNumber;
			std::vector<Event> warJustificationEvents;
			std::vector<Event> electionEvents;
			int electionEventNumber;
	};
}



#endif // HOI4_EVENTS_H