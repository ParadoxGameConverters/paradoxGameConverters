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



#ifndef EU4_HISTORY_H
#define EU4_HISTORY_H



#include "newParser.h"
#include "EU4Leader.h"
#include <memory>
#include <string>
#include <vector>



namespace EU4
{
	class historyItem
	{
		public:
		historyItem();

		std::string getType() const { return type; }

		protected:
		std::string type;
	};


	class historyDate: public commonItems::parser
	{
		public:
		historyDate();

		std::vector<std::shared_ptr<historyItem>> getItems() { return items; }

		private:
		std::vector<std::shared_ptr<historyItem>> items;
	};


	class history: public commonItems::parser
	{
		public:
		history();

		std::vector<std::shared_ptr<historyItem>> getItems() const { return items; }

		std::vector<std::shared_ptr<historyItem>> getItemsOfType(const std::string& type) const;

		private:
		std::vector<std::shared_ptr<historyItem>> items;
	};
}



#endif // EU4_HISTORY_H