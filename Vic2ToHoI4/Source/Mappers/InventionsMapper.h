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



#ifndef INVENTIONS_MAPPER_H
#define INVENTIONS_MAPPER_H



#include <optional>
#include <map>
#include <string>
using namespace std;



class inventionsMapper
{
	public:
		static optional<string> getInventionName(int inventionNum)
		{
			return getInstance()->GetInventionName(inventionNum);
		}
	private:
		static inventionsMapper* instance;
		static inventionsMapper* getInstance()
		{
			if (instance == nullptr)
			{
				instance = new inventionsMapper();
			}
			return instance;
		}
		inventionsMapper();

		inventionsMapper(const inventionsMapper&) = delete;
		inventionsMapper& operator=(const inventionsMapper&) = delete;

		string getInventionPath();
		void generateNums(string path);
		void processTechFile(string filename);

		optional<string> GetInventionName(int inventionNum);

		map<int, string> inventionNumsToNames;
};




#endif // INVENTIONS_MAPPER_H
