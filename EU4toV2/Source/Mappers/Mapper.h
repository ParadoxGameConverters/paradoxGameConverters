/*Copyright (c) 2016 The Paradox Game Converters Project

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



#ifndef MAPPER_H
#define MAPPER_H



#include "../FlagUtils.h"
#include <map>
#include <string>
#include <vector>
#include <memory>
using namespace std;



struct CustomFlag;
class Object;



// colonial nations
typedef struct {
	string tag;
	string EU4Region;
	string V2Region;
	string cultureGroup;
} colonyStruct;
typedef vector<colonyStruct> colonyMapping;
colonyMapping initColonyMap(Object* obj);

// colonial nation flags
typedef struct {
	std::string name;
	string region;
	bool unique;
	std::string overlord;
} colonyFlag;
typedef map<string, shared_ptr<colonyFlag> > colonyFlagset; // <name, flag>
colonyFlagset initColonyFlagset(Object* obj);

typedef map<string, shared_ptr<CustomFlag> > customFlagset; // <name, flag>


// utility functions
string CardinalToOrdinal(int cardinal);

#endif // MAPPER_H
