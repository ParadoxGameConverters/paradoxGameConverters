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



#ifndef RELIGION_MAPPER_H
#define RELIGION_MAPPER_H



#include <map>
#include <memory>
#include <string>
using namespace std;



class Object;



class religionMapper
{
	public:
		static string getVic2Religion(const string& EU4Religion)
		{
			return getInstance()->GetVic2Religion(EU4Religion);
		}
	private:
		static religionMapper* instance;
		static religionMapper* getInstance()
		{
			if (instance == nullptr)
			{
				instance = new religionMapper;
			}
			return instance;
		}

		religionMapper();
		void initReligionMap(shared_ptr<Object> obj);

		string GetVic2Religion(const string& EU4Religion);

		map<string, string> EU4ToVic2ReligionMap;
};



#endif // RELIGION_MAPPER_H
