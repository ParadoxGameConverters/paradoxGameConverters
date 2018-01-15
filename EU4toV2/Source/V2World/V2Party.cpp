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



#include "V2Party.h"
#include "Log.h"



V2Party::V2Party(shared_ptr<Object> party_obj)
{
	auto leaf = party_obj->getLeaf("name");
	if (leaf)
	{
		name = *leaf;
	}
	else
	{
		LOG(LogLevel::Error) << "Party did not have a name";
		exit(-1);
	}

	leaf = party_obj->getLeaf("ideology");
	if (leaf)
	{
		ideology = *leaf;
	}
	else
	{
		LOG(LogLevel::Error) << "Party " << name << " did not have an ideology";
		exit(-1);
	}

	leaf = party_obj->getLeaf("start_date");
	if (leaf)
	{
		start_date = date(*leaf);
	}
	else
	{
		LOG(LogLevel::Error) << "Party " << name << " did not have a start date";
		exit(-1);
	}

	leaf = party_obj->getLeaf("end_date");
	if (leaf)
	{
		end_date = date(*leaf);
	}
	else
	{
		LOG(LogLevel::Error) << "Party " << name << " did not have an end date";
		exit(-1);
	}

	leaf = party_obj->getLeaf("economic_policy");
	if (leaf)
	{
		economic_policy = *leaf;
	}
	else
	{
		LOG(LogLevel::Error) << "Party " << name << " did not have an economic policy";
		exit(-1);
	}

	leaf = party_obj->getLeaf("trade_policy");
	if (leaf)
	{
		trade_policy = *leaf;
	}
	else
	{
		LOG(LogLevel::Error) << "Party " << name << " did not have a trade policy";
		exit(-1);
	}

	leaf = party_obj->getLeaf("religious_policy");
	if (leaf)
	{
		religious_policy = *leaf;
	}
	else
	{
		LOG(LogLevel::Error) << "Party " << name << " did not have a religious policy";
		exit(-1);
	}

	leaf = party_obj->getLeaf("citizenship_policy");
	if (leaf)
	{
		citizenship_policy = *leaf;
	}
	else
	{
		LOG(LogLevel::Error) << "Party " << name << " did not have a citizenship policy";
		exit(-1);
	}

	leaf = party_obj->getLeaf("war_policy");
	if (leaf)
	{
		war_policy = *leaf;
	}
	else
	{
		LOG(LogLevel::Error) << "Party " << name << " did not have a war policy";
		exit(-1);
	}
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
