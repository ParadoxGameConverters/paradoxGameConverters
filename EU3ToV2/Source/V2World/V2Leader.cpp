/*Copyright (c) 2014 The Paradox Game Converters Project

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
#include "V2LeaderTraits.h"
#include "V2Country.h"

#include "../EU3World/EU3Leader.h"



V2Leader::V2Leader(const EU3Leader* oldLeader, const V2LeaderTraits& traits)
{
	name				= oldLeader->getName();
	activationDate	= oldLeader->getActivationDate();
	isLand			= oldLeader->isLand();
	personality		= traits.getPersonality( oldLeader->getFire(), oldLeader->getShock(), oldLeader->getManuever(), oldLeader->getSiege() );
	background		= traits.getBackground(  oldLeader->getFire(), oldLeader->getShock(), oldLeader->getManuever(), oldLeader->getSiege() );
}


void V2Leader::output(FILE *output) const
{
	fprintf(output, "leader = {\n");
	fprintf(output, "\tname=\"%s\"\n", name.c_str());
	fprintf(output, "\tdate=\"%s\"\n", activationDate.toString().c_str());
	if (isLand)
	{
		fprintf(output, "\ttype=land\n");
	}
	else
	{
		fprintf(output, "\ttype=sea\n");
	}
	fprintf(output, "\tpersonality=\"%s\"\n", personality.c_str());
	fprintf(output, "\tbackground=\"%s\"\n", background.c_str());
	fprintf(output, "}\n");
	fprintf(output, "\n");
}
