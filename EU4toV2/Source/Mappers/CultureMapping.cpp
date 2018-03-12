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



#include "CultureMapping.h"
#include "../EU4World/Regions.h"
#include "Log.h"



mappers::cultureMapping::cultureMapping(const std::string& _sourceCulture, const std::string& _destinationCulture, const std::map<std::string, std::string>& _distinguishers):
	sourceCulture(_sourceCulture),
	destinationCulture(_destinationCulture),
	distinguishers(_distinguishers)
{
}


bool mappers::cultureMapping::cultureMatch(const std::string& _sourceCulture, std::string& _destinationCulture, const std::string& religion, int EU4Province, const std::string& ownerTag)
{
	if (sourceCulture == _sourceCulture)
	{
		if (distinguishersMatch(distinguishers, religion, EU4Province, ownerTag))
		{
			_destinationCulture = destinationCulture;
			return true;
		}
	}

	return false;
}


bool mappers::cultureMapping::distinguishersMatch(const std::map<std::string, std::string>& distinguishers, const std::string& religion, int EU4Province, const std::string& ownerTag)
{
	for (auto currentDistinguisher: distinguishers)
	{
		if (currentDistinguisher.first == "owner")
		{
			if (ownerTag != currentDistinguisher.second)
			{
				return false;
			}
		}
		else if (currentDistinguisher.first == "religion")
		{
			if (religion != currentDistinguisher.second)
			{
				return false;
			}
		}
		else if (currentDistinguisher.first == "region")
		{
			if (!EU4::Regions::provinceInRegion(EU4Province, currentDistinguisher.second))
			{
				return false;
			}
		}
		else if (currentDistinguisher.first == "provinceid")
		{
			if (stoi(currentDistinguisher.second) != EU4Province)
			{
				return false;
			}
		}
		else
		{
			LOG(LogLevel::Warning) << "Unhandled distinguisher type in culture rules: " << currentDistinguisher.first;
			return false;
		}
	}

	return true;
}