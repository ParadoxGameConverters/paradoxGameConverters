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



#ifndef CUSTOM_FLAGSET_MAPPER_H
#define CUSTOM_FLAGSET_MAPPER_H



#include "newParser.h"
#include <map>
#include <memory>
#include <string>



namespace mappers
{
	class colonyFlag: commonItems::parser
	{
		public:
			colonyFlag(std::istream& theStream, const std::string& region);

			std::string getName() const { return name; }
			std::string getRegion() const { return region; }
			bool isUnique() const { return unique; }
			std::string getOverlord() const { return overlord; }

			void setOverlord(const std::string& newOverlord) { overlord = newOverlord; }

		private:
			std::string name;
			std::string region;
			bool unique;
			std::string overlord;
	};


	class colonyFlagsetRegion: commonItems::parser
	{
		public:
			colonyFlagsetRegion(std::istream& theStream, const std::string& region, std::map<std::string, std::shared_ptr<colonyFlag>>& colonyFlagset);
	};


	class colonyFlagsetMapper: commonItems::parser
	{
		public:
			static std::shared_ptr<colonyFlag> getFlag(const std::string& name)
			{
				return getInstance()->GetFlag(name);
			}

			static std::vector<std::string> getNames()
			{
				return getInstance()->GetNames();
			}

			static void removeFlag(const std::string& name)
			{
				getInstance()->RemoveFlag(name);
			}

		private:
			static colonyFlagsetMapper* instance;
			static colonyFlagsetMapper* getInstance()
			{
				if (instance == nullptr)
				{
					instance = new colonyFlagsetMapper;
				}
				return instance;
			}
			colonyFlagsetMapper();

			std::shared_ptr<colonyFlag> GetFlag(const std::string& name);
			std::vector<std::string> GetNames();
			void RemoveFlag(const std::string& name);

			std::map<std::string, std::shared_ptr<colonyFlag>> colonyFlagset;
	};
}




#endif // CUSTOM_FLAGSET_MAPPER_H
