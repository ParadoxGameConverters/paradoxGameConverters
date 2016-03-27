/*Copyright (c) 2015 The Paradox Game Converters Project

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
#include "../Log.h"
#include "../Parsers/Parser.h"



V2Regiment::V2Regiment (Object* obj)
{
	string key = obj->getKey ();

	std::vector<Object*> objName = obj->getValue ("name");
	(objName.size () > 0) ? name = objName[0]->getLeaf () : name = "";

	std::vector<Object*> objType = obj->getValue ("type");
	if (objType.size () > 0)
	{
		type = objType[0]->getLeaf ();
	}
	else
	{
		LOG(LogLevel::Warning) << "Regiment or Ship " << name << " has no type";
		type = "";
	}

	std::vector<Object*> objStr = obj->getValue ("strength");
	if (objStr.size () > 0)
	{
		strength = atof (objStr[0]->getLeaf ().c_str ());
	}
	else
	{
		strength = 0.0;
	}

	objStr = obj->getValue ("organisation");
	if (objStr.size () > 0)
	{
		organization = atof (objStr[0]->getLeaf ().c_str ());
	}
	else
	{
		organization = 0.0;
	}

	objStr = obj->getValue ("experience");
	if (objStr.size () > 0)
	{
		experience = atof (objStr[0]->getLeaf ().c_str ());
	}
	else
	{
		experience = 0.0;
	}
}


V2Army::V2Army (Object* obj)
{
	string key = obj->getKey ();
	isNavy = (key == "navy");

	std::vector<Object*> objName = obj->getValue ("name");
	(objName.size () > 0) ? name = objName[0]->getLeaf () : name = "";

	std::vector<Object*> objLoc = obj->getValue ("location");
	if (objLoc.size () > 0)
	{
		location = atoi (objLoc[0]->getLeaf ().c_str ());
	}
	else
	{
		LOG (LogLevel::Warning) << "Army or Navy " << name << " has no location";
		location = -1;
	}

	std::vector<Object*> objAtSea = obj->getValue ("at_sea");
	(objAtSea.size () > 0) ? at_sea = atoi (objAtSea[0]->getLeaf ().c_str ()) : at_sea = 0;

	regiments.clear ();
	std::vector<Object*> objRegs = obj->getValue ("regiment");
	for (vector<Object*>::iterator itr = objRegs.begin (); itr != objRegs.end (); ++itr)
	{
		V2Regiment* newRegiment = new V2Regiment(*itr);
		regiments.push_back(newRegiment);
	}
	std::vector<Object*> objShips = obj->getValue ("ship");
	for (vector<Object*>::iterator itr = objShips.begin (); itr != objShips.end (); ++itr)
	{
		V2Regiment* newShip = new V2Regiment(*itr);
		regiments.push_back (newShip);
	}

	std::vector<Object*> objSupp = obj->getValue ("supplies");
	if (objSupp.size () > 0)
	{
		supplies = atof (objSupp[0]->getLeaf ().c_str ());
	}
	else
	{
		supplies = 0.0;
	}
}