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



#include "Resources.h"
#include "ParserHelpers.h"


class actualResources: commonItems::parser
{
	public:
		actualResources(std::istream& theStream);

		auto getResources() const { return theResources; }

	private:
		std::map<std::string, double> theResources;
};


actualResources::actualResources(std::istream& theStream)
{
	registerKeyword(std::regex("[a-z]+"), [this](const std::string& resourceName, std::istream& theStream){
		commonItems::singleDouble resourceAmount(theStream);
		theResources.insert(std::make_pair(resourceName, resourceAmount.getDouble()));
	});

	parseStream(theStream);
}


class link: commonItems::parser
{
	public:
		link(std::istream& theStream);

		auto getProvinceNum() const { return provinceNum; }
		auto getResources() const { return theResources; }

	private:
		int provinceNum = 0;
		std::map<std::string, double> theResources;
};


link::link(std::istream& theStream)
{
	registerKeyword(std::regex("province"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt theProvinceNum(theStream);
		provinceNum = theProvinceNum.getInt();
	});
	registerKeyword(std::regex("resources"), [this](const std::string& unused, std::istream& theStream){
		actualResources theActualResources(theStream);
		theResources = theActualResources.getResources();
	});

	parseStream(theStream);
}


resources::resources() noexcept
{
	registerKeyword(std::regex("link"), [this](const std::string& unused, std::istream& theStream){
		link theLink(theStream);
		resourceMap.insert(std::make_pair(theLink.getProvinceNum(), theLink.getResources()));
	});

	parseFile("resources.txt");
}


std::map<std::string, double> resources::getResourcesInProvince(int provinceNum)
{
	if (auto mapping = resourceMap.find(provinceNum); mapping != resourceMap.end())
	{
		return mapping->second;
	}
	else
	{
		return {};
	}
}