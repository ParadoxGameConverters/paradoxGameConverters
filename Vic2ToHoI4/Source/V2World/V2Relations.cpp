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



#include "V2Relations.h"
#include "Object.h"



V2Relations::V2Relations(shared_ptr<Object> relationsObj):
	tag(relationsObj->getKey()),
	value(relationsObj->safeGetInt("value")),
	level(relationsObj->safeGetInt("level", 2)),
	militaryAccess(relationsObj->safeGetString("military_access", "no") == "yes"),
	lastSentDiplomat(relationsObj->safeGetObject("last_send_diplomat")),
	lastWar(relationsObj->safeGetObject("last_war")),
	truceUntil(relationsObj->safeGetObject("truce_until"))
{
}