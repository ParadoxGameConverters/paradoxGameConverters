///*Copyright (c) 2015 The Paradox Game Converters Project
//
//Permission is hereby granted, free of charge, to any person obtaining
//a copy of this software and associated documentation files (the
//"Software"), to deal in the Software without restriction, including
//without limitation the rights to use, copy, modify, merge, publish,
//distribute, sublicense, and/or sell copies of the Software, and to
//permit persons to whom the Software is furnished to do so, subject to
//the following conditions:
//
//The above copyright notice and this permission notice shall be included
//in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
//CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#include "V2Party.h"
#include "..\Log.h"



V2Party::V2Party(Object *party_obj)
{
	name						= party_obj->getLeaf("name");
	ideology					= party_obj->getLeaf("ideology");
	economic_policy		= party_obj->getLeaf("economic_policy");
	trade_policy			= party_obj->getLeaf("trade_policy");
	religious_policy		= party_obj->getLeaf("religious_policy");
	citizenship_policy	= party_obj->getLeaf("citizenship_policy");
	war_policy				= party_obj->getLeaf("war_policy");
}
