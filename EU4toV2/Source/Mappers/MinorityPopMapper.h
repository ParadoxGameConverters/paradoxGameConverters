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



#ifndef MINORITY_POP_MAPPER_H
#define MINORITY_POP_MAPPER_H



#include <memory>
#include <string>
#include <vector>
using namespace std;



class Object;
class V2Pop;



class minorityPopMapper
{
	public:
		static bool matchMinorityPop(V2Pop* pop)
		{
			return getInstance()->MatchMinorityPop(pop);
		}

	private:
		static minorityPopMapper* instance;
		static minorityPopMapper* getInstance()
		{
			if (instance == nullptr)
			{
				instance = new minorityPopMapper;
			}
			return instance;
		}

		minorityPopMapper();
		void initMinorityPopMap(shared_ptr<Object> obj);

		bool MatchMinorityPop(V2Pop* pop);

		vector<pair<string, string>> minorityPopMap;
};




#endif // MINORITY_POP_MAPPER_H
