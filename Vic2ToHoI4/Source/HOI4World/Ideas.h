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



#ifndef HOI4_IDEAS_H
#define HOI4_IDEAS_H


#include "IdeaGroup.h"
#include "newParser.h"
#include "NewParserToOldParserConverters.h"
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>



namespace HoI4
{

class Ideas: commonItems::parser
{
	public:
		Ideas();

		void updateIdeas(std::set<std::string> majorIdeologies);

		void output(std::set<std::string> majorIdeologies) const;

	private:
		void importIdeologicalIdeas();
		void importGeneralIdeas();

		void outputIdeologicalIdeas(std::set<std::string> majorIdeologies) const;
		void outputGeneralIdeas() const;

		std::ofstream openIdeaFile(const std::string& fileName) const;
		void closeIdeaFile(std::ofstream& fileStream) const;

		std::map<std::string, std::vector<std::shared_ptr<Object>>> ideologicalIdeas;
		std::vector<std::unique_ptr<IdeaGroup>> generalIdeas;
};

}



#endif // HOI4_IDEAS_H