/*Copyright (c) 2018 The Paradox Game Converters Project

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



#include "Event.h"
#include "ParserHelpers.h"



HoI4::Event::Event(const std::string& theType, std::istream& theStream):
	type(theType)
{
	registerKeyword(std::regex("id"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString idString(theStream);
			id = idString.getString();
		}
	);
	registerKeyword(std::regex("title"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString titleString(theStream);
			title = titleString.getString();
		}
	);
	registerKeyword(std::regex("desc"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::stringOfItem descriptionString(theStream);
			descriptions.push_back("desc " + descriptionString.getString());
		}
	);
	registerKeyword(std::regex("picture"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString pictureString(theStream);
			picture = pictureString.getString();
		}
	);
	registerKeyword(std::regex("major"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString majorString(theStream);
			majorEvent = (majorString.getString() == "yes");
		}
	);
	registerKeyword(std::regex("is_triggered_only"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString triggeredString(theStream);
			triggeredOnly = (triggeredString.getString() == "yes");
		}
	);
	registerKeyword(std::regex("hidden"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString hiddenString(theStream);
			hidden = (hiddenString.getString() == "yes");
		}
	);
	registerKeyword(std::regex("trigger"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::stringOfObject triggerString(theStream);
			trigger = triggerString.getString();
		}
	);
	registerKeyword(std::regex("mean_time_to_happen"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::stringOfObject MTTHString(theStream);
			meanTimeToHappen = MTTHString.getString();
		}
	);
	registerKeyword(std::regex("immediate"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::stringOfObject immediateString(theStream);
			immediate = immediateString.getString();
		}
	);
	registerKeyword(std::regex("option"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::stringOfObject optionString(theStream);
			options.push_back(optionString.getString());
		}
	);

	parseStream(theStream);
}
	
	
std::ofstream& HoI4::operator << (std::ofstream& out, const Event& theEvent)
{
	out << theEvent.type << " = {\n";
	out << "	id = " << theEvent.id << "\n";
	out << "	title = " << theEvent.title << "\n";
	for (auto description: theEvent.descriptions)
	{
		out << "\t" << description << "\n";
	}
	out << "	picture = " << theEvent.picture << "\n";
	if (theEvent.majorEvent)
	{
		out << "	\n";
		out << "	major = yes\n";
	}
	out << "\n";
	if (theEvent.triggeredOnly)
	{
		out << "	is_triggered_only = yes\n";
	}
	if (theEvent.hidden)
	{
		out << "	hidden = yes\n";
	}

	if (theEvent.trigger != "")
	{
		out << "\n";
		out << "	trigger " << theEvent.trigger << "\n";
	}

	if (theEvent.meanTimeToHappen != "")
	{
		out << "\n";
		out << "	mean_time_to_happen " << theEvent.meanTimeToHappen << "\n";
	}

	if (theEvent.immediate != "")
	{
		out << "\n";
		out << "	immediate " << theEvent.immediate << "\n";
	}

	for (auto option: theEvent.options)
	{
		out << "\n";
		out << "	option " << option << "\n";
	}

	out << "}\n";

	return out;
}