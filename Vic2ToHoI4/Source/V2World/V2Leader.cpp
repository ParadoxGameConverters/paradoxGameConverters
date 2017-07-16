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



#include "V2Leader.h"



V2Leader::V2Leader(shared_ptr<Object> obj)
{
	name = obj->getLeaf("name");
	if (name[0] == '\"')
	{
		name	= name.substr(1, name.length() - 2);
	}

	type = obj->getLeaf("type");

	personality	= obj->getLeaf("personality");
	if (personality[0] == '\"')
	{
		personality	= personality.substr(1, personality.length() - 2);
	}

	background	= obj->getLeaf("background");
	if (background[0] == '\"')
	{
		background	= background.substr(1, background.length() - 2);
	}

	prestige = stof(obj->getLeaf("prestige"));
}