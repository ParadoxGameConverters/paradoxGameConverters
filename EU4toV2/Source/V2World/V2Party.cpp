#include "V2Party.h"

#include "..\Log.h"

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

V2Party::V2Party(const std::string& name, const std::string& ideology)
: name(name), ideology(ideology)
{
	if (ideology == "conservative")
	{
		start_date = date("1820.1.1");
		end_date = date("2000.1.1");
		economic_policy = "interventionism";
		trade_policy = "protectionism";
		religious_policy = "moralism";
		citizenship_policy = "limited_citizenship";
		war_policy = "pro_military";
	}
	else if (ideology == "liberal")
	{
		start_date = date("1820.1.1");
		end_date = date("2000.1.1");
		economic_policy = "laissez_faire";
		trade_policy = "free_trade";
		religious_policy = "pluralism";
		citizenship_policy = "limited_citizenship";
		war_policy = "anti_military";
	}
	else if (ideology == "reactionary")
	{
		start_date = date("1820.1.1");
		end_date = date("2000.1.1");
		economic_policy = "state_capitalism";
		trade_policy = "protectionism";
		religious_policy = "moralism";
		citizenship_policy = "residency";
		war_policy = "jingoism";
	}
	else if (ideology == "socialist")
	{
		start_date = date("1849.1.1");
		end_date = date("2000.1.1");
		economic_policy = "state_capitalism";
		trade_policy = "free_trade";
		religious_policy = "secularized";
		citizenship_policy = "full_citizenship";
		war_policy = "anti_military";
	}
	else if (ideology == "communist")
	{
		start_date = date("1849.1.1");
		end_date = date("2000.1.1");
		economic_policy = "planned_economy";
		trade_policy = "protectionism";
		religious_policy = "pro_atheism";
		citizenship_policy = "full_citizenship";
		war_policy = "pro_military";
	}
	else if (ideology == "anarcho_liberal")
	{
		start_date = date("1830.1.1");
		end_date = date("2000.1.1");
		economic_policy = "laissez_faire";
		trade_policy = "free_trade";
		religious_policy = "pro_atheism";
		citizenship_policy = "full_citizenship";
		war_policy = "pro_military";
	}
	else if (ideology == "fascist")
	{
		start_date = date("1905.1.1");
		end_date = date("2000.1.1");
		economic_policy = "state_capitalism";
		trade_policy = "protectionism";
		religious_policy = "moralism";
		citizenship_policy = "residency";
		war_policy = "jingoism";
	}
	else
	{
		LOG(LogLevel::Warning) << "Unknown party ideology \"" << ideology << "\" for party \"" << name << '"';
	}
}
