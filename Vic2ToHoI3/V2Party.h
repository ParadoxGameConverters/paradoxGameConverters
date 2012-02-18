#include "Parsers/Object.h"

#include "Date.h"

struct V2Party
{
public:
	V2Party(Object* party_obj);
	V2Party() : name("NULL PARTY") {};

	string name;
	string ideology;

	date start_date;
	date end_date;
	
	string economic_policy;
	string trade_policy;
	string religious_policy;
	string citizenship_policy;
	string war_policy;

	bool isActiveOn(date query) { return ((query > start_date) && (query < end_date)); };
};
