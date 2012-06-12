#include "V2Party.h"

V2Party::V2Party(Object *party_obj)
{
	name = party_obj->getLeaf("name");
	ideology = party_obj->getLeaf("ideology");

	start_date = date(party_obj->getLeaf("start_date"));
	end_date = date(party_obj->getLeaf("end_date"));

	economic_policy = party_obj->getLeaf("economic_policy");
	trade_policy = party_obj->getLeaf("trade_policy");
	religious_policy = party_obj->getLeaf("religious_policy");
	citizenship_policy = party_obj->getLeaf("citizenship_policy");
	war_policy = party_obj->getLeaf("war_policy");
}