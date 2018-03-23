/*Copyright(c) 2018 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */



#ifndef EU4_COUNTRY_HISTORY_H
#define EU4_COUNTRY_HISTORY_H



#include "History.h"



namespace EU4
{
	class historyLeader: public historyItem
	{
		public:
		historyLeader(std::istream& theStream);

		std::shared_ptr<EU4::leader> getTheLeader() { return theLeader; }

		private:
		std::shared_ptr<EU4::leader> theLeader;
	};


	class countryHistoryDate: public historyDate
	{
		public:
		countryHistoryDate(std::istream& theStream);

		private:
		std::vector<std::shared_ptr<historyItem>> items;
	};


	class countryHistory: public history
	{
		public:
		countryHistory(std::istream& theStream);

		private:
		std::vector<std::shared_ptr<historyItem>> items;
	};
}



#endif // EU4_COUNTRY_HISTORY_H