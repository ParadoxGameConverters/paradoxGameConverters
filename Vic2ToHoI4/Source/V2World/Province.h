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



#ifndef VIC2_PROVINCE_H_
#define VIC2_PROVINCE_H_



#include "../Configuration.h"
#include "newParser.h"
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>



namespace Vic2
{

class Country;
class Pop;


class Province: commonItems::parser
{
	public:
		explicit Province(const std::string& numberString, std::istream& theStream);
		void setCores(const std::map<std::string, Country*>& countries);

		int getTotalPopulation() const;
		int getPopulation(std::optional<std::string> type = {}) const;
		int getLiteracyWeightedPopulation(std::optional<std::string> type = {}) const;
		double getPercentageWithCultures(const std::set<std::string>& cultures) const;

		void setOwner(const Country* _owner) { owner = _owner; }
		void addCoreString(const std::string& coreString) { coreStrings.insert(coreString); }
		void removeCoreString(const std::string& coreString) { coreStrings.erase(coreString); }
		void removeCore(Country* core) { cores.erase(core); }

		int getNumber() const { return number; }
		std::string getOwnerString() const { return ownerString; }
		const Country* getOwner() const { return owner; }
		std::set<Country*> getCores() const { return cores; }
		int getRailLevel() const { return railLevel; }
		int getFortLevel() const { return fortLevel; }
		int getNavalBaseLevel() const { return navalBaseLevel; }
		std::vector<std::shared_ptr<const Pop>> getPops() const { return pops; }

	private:
		Province(const Province&) = delete;
		Province& operator=(const Province&) = delete;

		int calculateLiteracyWeightedPop(const std::shared_ptr<const Pop> thePop) const;

		int number;

		std::string ownerString = "";
		const Country* owner = nullptr;

		std::set<std::string> coreStrings;
		std::set<Country*> cores;

		std::vector<std::shared_ptr<const Pop>> pops;

		int fortLevel = 0;
		int navalBaseLevel = 0;
		int railLevel = 0;
};

}



#endif // VIC2_PROVINCE_H_
