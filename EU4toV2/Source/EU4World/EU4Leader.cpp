/*Copyright(c) 2018 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */



#include "EU4Leader.h"
#include "ID.h"
#include "../Configuration.h"
#include "Log.h"



EU4::leader::leader(std::istream& theStream):
	name(),
	type(),
	female(false),
	fire(0),
	shock(0),
	manuever(0),
	siege(0),
	country(),
	personality(),
	activationDate(),
	deathDate(),
	id()
{
	registerKeyword(std::regex("name"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString nameString(theStream);
			name = nameString.getString();
		}
	);
	registerKeyword(std::regex("type"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString typeString(theStream);
			type = typeString.getString();
		}
	);
	registerKeyword(std::regex("female"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString unusedString(theStream);
			female = true;
		}
	);
	registerKeyword(std::regex("manuever"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleInt theValue(theStream);
			manuever = theValue.getInt();
		}
	);
	registerKeyword(std::regex("fire"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleInt theValue(theStream);
			fire = theValue.getInt();
		}
	);
	registerKeyword(std::regex("shock"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleInt theValue(theStream);
			shock = theValue.getInt();
		}
	);
	registerKeyword(std::regex("siege"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleInt theValue(theStream);
			siege = theValue.getInt();
		}
	);
	registerKeyword(std::regex("country"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString countryString(theStream);
			country = countryString.getString();
		}
	);
	registerKeyword(std::regex("personality"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString personalityString(theStream);
			personality = personalityString.getString();
		}
	);
	registerKeyword(std::regex("activation"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString dateString(theStream);
			date anActivationDate(dateString.getString());
			activationDate = anActivationDate;
		}
	);
	registerKeyword(std::regex("death_date"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString dateString(theStream);
			date aDeathDate(dateString.getString());
			deathDate = aDeathDate;
		}
	);
	registerKeyword(std::regex("id"), [this](const std::string& unused, std::istream& theStream)
		{
			ID theID(theStream);
			id = theID.getIDNum();
		}
	);
	registerKeyword(std::regex("monarch_id"), [this](const std::string& unused, std::istream& theStream)
		{
			ID theID(theStream);
			monarchID = theID.getIDNum();
		}
	);
	registerKeyword(std::regex("dynasty"), commonItems::ignoreString);

	parseStream(theStream);
}


bool EU4::leader::isLand() const
{
	if (type == "general" || type == "conquistador")
	{
		return true;
	}
	else if (type == "explorer" || type == "admiral")
	{
		return false;
	}
	else
	{
		LOG(LogLevel::Warning) << "Unknown leader type " << type;
		return false;
	}
}


bool EU4::leader::isAlive() const
{
	if (deathDate < Configuration::getLastEU4Date())
	{
		return false;
	}
	else
	{
		return true;
	}
}