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



#ifndef STATE_MAPPER_H
#define STATE_MAPPER_H



#include <map>
#include <memory>
#include <vector>
using namespace std;



class Object;



class stateMapper
{
	public:
		static vector<int> getOtherProvincesInState(int province)
		{
			return getInstance()->GetOtherProvincesInState(province);
		}

		static int getStateIndex(int province)
		{
			return getInstance()->GetStateIndex(province);
		}

	private:
		static stateMapper* instance;
		static stateMapper* getInstance()
		{
			if (instance == NULL)
			{
				instance = new stateMapper();
			}
			return instance;
		}

		stateMapper();
		void initStateMap(shared_ptr<Object> obj);

		vector<int> GetOtherProvincesInState(int province);
		int GetStateIndex(int province);

		map<int, vector<int>> stateProvincesMap;
		map<int, int> stateIndexMap;
};



#endif // STATE_MAPPER_H