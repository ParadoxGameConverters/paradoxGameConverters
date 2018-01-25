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



#include "V2Army.h"
#include "Log.h"
#include "Object.h"



V2Regiment::V2Regiment(shared_ptr<Object> obj):
	name(obj->safeGetString("name")),
	type(obj->safeGetString("type")),
	strength(obj->safeGetFloat("strength")),
	organization(obj->safeGetFloat("organisation")),
	experience(obj->safeGetFloat("experience"))
{
	if (type == "")
	{
		LOG(LogLevel::Warning) << "Regiment or Ship " << name << " has no type";
	}
}


V2Army::V2Army(shared_ptr<Object> obj):
	name(obj->safeGetString("name")),
	location(obj->safeGetInt("location", location)),
	regiments(),
	supplies(obj->safeGetFloat("supplies")),
	at_sea(obj->safeGetInt("at_sea")),
	navy(obj->getKey() == "navy")
{
	if (location == -1)
	{
		LOG(LogLevel::Warning) << "Army or Navy " << name << " has no location";
	}

	regiments.clear();
	for (auto regimentObj: obj->getValue("regiment"))
	{
		V2Regiment* newRegiment = new V2Regiment(regimentObj);
		regiments.push_back(newRegiment);
	}

	for (auto shipObj: obj->getValue("ship"))
	{
		V2Regiment* newShip = new V2Regiment(shipObj);
		regiments.push_back(newShip);
	}
}