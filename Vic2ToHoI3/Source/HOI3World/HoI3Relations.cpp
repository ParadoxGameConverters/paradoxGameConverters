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



#include "HoI3Relations.h"
#include "../V2World/V2Relations.h"



HoI3Relations::HoI3Relations(string newTag)
{
	tag					= newTag;
	value					= 0;
	militaryAccess		= false;
	lastSendDiplomat	= date();
	lastWar				= date();
	truceUntil = date();
	guarantee = false;
}


HoI3Relations::HoI3Relations(string newTag, V2Relations* oldRelations)
{
	tag					= newTag;
	value					= oldRelations->getRelations();
	militaryAccess		= oldRelations->hasMilitaryAccess();
	lastSendDiplomat	= oldRelations->getDiplomatLastSent();
	lastWar				= oldRelations->getLastWar();
	truceUntil = oldRelations->getTruceUntil();
	guarantee = (oldRelations->getLevel() >= 4);
}

