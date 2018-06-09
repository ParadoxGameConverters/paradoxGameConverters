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



#ifndef HOI4_FOCUS_TREE
#define HOI4_FOCUS_TREE

#include "HoI4World.h"
#include "Events.h"
#include "newParser.h"
#include <memory>
#include <set>
#include <string>
#include <vector>



class HoI4Country;
class HoI4Focus;
namespace HoI4
{
	class Events;
}



class HoI4FocusTree: commonItems::parser
{
	public:
		HoI4FocusTree() = default;
		explicit HoI4FocusTree(const HoI4Country& country);

		shared_ptr<HoI4FocusTree> makeCustomizedCopy(const HoI4Country& country) const;
		void setNextFreeColumn(int newFreeColumn) { nextFreeColumn = newFreeColumn; }

		void addGenericFocusTree(const std::set<std::string>& majorIdeologies);

		void addDemocracyNationalFocuses(std::shared_ptr<HoI4Country> Home, std::vector<std::shared_ptr<HoI4Country>>& CountriesToContain);
		void addAbsolutistEmpireNationalFocuses(std::shared_ptr<HoI4Country> country, const std::vector<std::shared_ptr<HoI4Country>>& targetColonies, const std::vector<std::shared_ptr<HoI4Country>>& annexationTargets);
		void addCommunistCoupBranch(std::shared_ptr<HoI4Country> Home, const std::vector<std::shared_ptr<HoI4Country>>& coupTargets, const std::set<std::string>& majorIdeologies);
		void addCommunistWarBranch(std::shared_ptr<HoI4Country> Home, const std::vector<std::shared_ptr<HoI4Country>>& warTargets, HoI4::Events* events);
		void addFascistAnnexationBranch(std::shared_ptr<HoI4Country> Home, const std::vector<std::shared_ptr<HoI4Country>>& annexationTargets, HoI4::Events* events);
		void addFascistSudetenBranch(std::shared_ptr<HoI4Country> Home, const std::vector<std::shared_ptr<HoI4Country>>& sudetenTargets, const std::vector<std::vector<int>>& demandedStates, const HoI4World* world);
		void addGPWarBranch(std::shared_ptr<HoI4Country> Home, const std::vector<std::shared_ptr<HoI4Country>>& newAllies, const std::vector<std::shared_ptr<HoI4Country>>& GCTargets, const std::string& ideology, HoI4::Events* events);
		void removeFocus(const std::string& id);

		void output(const std::string& filename) const;

		void addFocus(std::shared_ptr<HoI4Focus> newFocus) { focuses.push_back(newFocus); }

	private:
		HoI4FocusTree(const HoI4FocusTree&) = delete;
		HoI4FocusTree& operator=(const HoI4FocusTree&) = delete;

		void confirmLoadedFocuses();

		int calculateNumCollectovistIdeologies(const std::set<std::string>& majorIdeologies);
		void determineMutualExclusions(const std::set<std::string>& majorIdeologies);
		void addFascistGenericFocuses(int relativePosition, const std::set<std::string>& majorIdeologies);
		void addCommunistGenericFocuses(int relativePosition);
		void addAbsolutistGenericFocuses(int relativePosition);
		void addRadicalGenericFocuses(int relativePosition);

		std::string srcCountryTag = "";
		std::string dstCountryTag = "";
		std::vector<std::shared_ptr<HoI4Focus>> focuses;
		int nextFreeColumn = 0;

		std::string fascistMutualExlusions = "";
		std::string communistMutualExclusions = "";
		std::string absolutistMutualExlusions = "";
		std::string radicalMutualExclusions = "";
};



#endif // HOI4_FOCUS_TREE