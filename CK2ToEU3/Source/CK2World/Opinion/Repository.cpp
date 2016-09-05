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

#include "CK2World/Opinion/Repository.h"

void ck2::opinion::Repository::initOpinions(IObject* root)
{
	if (root == NULL)
	{
		return;
	}
	std::vector<IObject*> opinions = root->getLeaves();
	for (std::vector<IObject*>::iterator itr = opinions.begin(); itr != opinions.end(); ++itr)
	{
		std::string name = (*itr)->getKey();
		std::vector<IObject*> opinionObjs = (*itr)->getValue("opinion");
		int value = 0;
		if (opinionObjs.size() > 0)
		{
				value = atoi(opinionObjs[0]->getLeaf().c_str());
		}
		if (!name.empty() && value != 0)
		{
			opinionVals[name] = value;
		}
	}
}

int ck2::opinion::Repository::getBaseValue(std::string opinion)
{
	std::map<std::string,int>::const_iterator itr = opinionVals.find(opinion);
	if (itr == opinionVals.end())
		return 0;
	return itr->second;
}
