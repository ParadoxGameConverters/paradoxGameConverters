/*Copyright(c) 2014 The Paradox Game Converters Project

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


#include "EU4Relations.h"
#include "../Parsers/Object.h"



EU4Relations::EU4Relations(Object* obj) {
	tag = obj->getKey();

	vector<Object*> valueObj = obj->getValue("value");	// the object holding the relationship value
	(valueObj.size() > 0) ? value = atoi( valueObj[0]->getLeaf().c_str() ) : value = 0;

	vector<Object*> maObj = obj->getValue("military_access");	// the object holding the military access status
	(maObj.size() > 0) ? military_access = (maObj[0]->getLeaf() == "yes") : military_access = false;

	vector<Object*> lastSendObj = obj->getValue("last_send_diplomat");	// the object holding the date the last diplomat was sent
	(lastSendObj.size() > 0) ? last_send_diplomat = date(lastSendObj[0]) : last_send_diplomat = date();

	vector<Object*> lastWarObj = obj->getValue("last_war");	// the object holding the date of the last war
	(lastWarObj.size() > 0) ? last_war = date(lastWarObj[0]) : last_war = date();
}
