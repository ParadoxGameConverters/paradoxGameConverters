#include "V2Leader.h"
#include "V2LeaderTraits.h"
#include "V2Country.h"

#include "EU3Leader.h"



static int nextId = 0;


V2Leader::V2Leader(string tag, const EU3Leader* oldLeader, const V2LeaderTraits& traits)
{
	name				= oldLeader->getName();
	activationDate	= oldLeader->getActivationDate();
	isLand			= oldLeader->isLand();
	personality		= traits.getPersonality( oldLeader->getFire(), oldLeader->getShock(), oldLeader->getManuever(), oldLeader->getSiege() );
	background		= traits.getBackground(  oldLeader->getFire(), oldLeader->getShock(), oldLeader->getManuever(), oldLeader->getSiege() );
	country			= tag;
	//picture			= "";		// TODO: figure out picture
	id = ++nextId;
}


void V2Leader::output(FILE *output) const
{
	fprintf(output, "\tleader=\n");
	fprintf(output, "\t{\n");
	fprintf(output, "\t\tname=\"%s\"\n", name.c_str());
	fprintf(output, "\t\tdate=\"%s\"\n", activationDate.toString().c_str());
	if (isLand)
	{
		fprintf(output, "\t\ttype=land\n");
	}
	else
	{
		fprintf(output, "\t\ttype=sea\n");
	}
	fprintf(output, "\t\tpersonality=\"%s\"\n", personality.c_str());
	fprintf(output, "\t\tbackground=\"%s\"\n", background.c_str());
	fprintf(output, "\t\tcountry=\"%s\"\n", country.c_str());
	//fprintf(output, "\t\tpicture=\"%s\"\n", picture.c_str());
	fprintf(output, "\t\tid=\n");
	fprintf(output, "\t\t{\n");
	fprintf(output, "\t\t\tid=%d\n", id);
	fprintf(output, "\t\t\ttype=37\n");
	fprintf(output, "\t\t}\n");
	fprintf(output, "\t}\n");
}
