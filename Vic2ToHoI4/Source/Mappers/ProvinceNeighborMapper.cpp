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



#include "ProvinceNeighborMapper.h"
#include "ProvinceDefinitions.h"
#include "../Color.h"
#include "../Configuration.h"
#include "Log.h"



provinceNeighborMapper* provinceNeighborMapper::instance = nullptr;



provinceNeighborMapper::provinceNeighborMapper():
	provinceNeighbors(),
	borders(),
	provinceMap(Configuration::getHoI4Path() + "/map/provinces.bmp")
{
	if (!provinceMap)
	{
		LOG(LogLevel::Error) << "Could not open " << Configuration::getHoI4Path() << "/map/provinces.bmp";
	}

	unsigned int height = provinceMap.height();
	unsigned int width = provinceMap.width();

	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			point position = { x, y };

			ConverterColor::Color centerColor = getCenterColor(position);
			ConverterColor::Color aboveColor = getAboveColor(position, height);
			ConverterColor::Color belowColor = getBelowColor(position, height);
			ConverterColor::Color leftColor = getLeftColor(position, width);
			ConverterColor::Color rightColor = getRightColor(position, width);

			position.second = height - position.second - 1;
			if (centerColor != aboveColor)
			{
				handleNeighbor(centerColor, aboveColor, position);
			}
			if (centerColor != rightColor)
			{
				handleNeighbor(centerColor, rightColor, position);
			}
			if (centerColor != belowColor)
			{
				handleNeighbor(centerColor, belowColor, position);
			}
			if (centerColor != leftColor)
			{
				handleNeighbor(centerColor, leftColor, position);
			}
		}
	}
}


ConverterColor::Color provinceNeighborMapper::getCenterColor(point position)
{
	rgb_t color;
	provinceMap.get_pixel(position.first, position.second, color);

	ConverterColor::Color theColor(ConverterColor::red(color.red), ConverterColor::green(color.green), ConverterColor::blue(color.blue));
	return theColor;
}


ConverterColor::Color provinceNeighborMapper::getAboveColor(point position, int height)
{
	if (position.second > 0)
	{
		position.second--;
	}

	rgb_t color;
	provinceMap.get_pixel(position.first, position.second, color);

	ConverterColor::Color theColor(color.red, color.green, color.blue);
	return theColor;
}


ConverterColor::Color provinceNeighborMapper::getBelowColor(point position, int height)
{
	if (position.second < height - 1)
	{
		position.second++;
	}

	rgb_t color;
	provinceMap.get_pixel(position.first, position.second, color);

	ConverterColor::Color theColor(color.red, color.green, color.blue);
	return theColor;
}


ConverterColor::Color provinceNeighborMapper::getLeftColor(point position, int width)
{
	if (position.first > 0)
	{
		position.first--;
	}
	else
	{
		position.first = width - 1;
	}

	rgb_t color;
	provinceMap.get_pixel(position.first, position.second, color);

	ConverterColor::Color theColor(color.red, color.green, color.blue);
	return theColor;
}


ConverterColor::Color provinceNeighborMapper::getRightColor(point position, int width)
{
	if (position.first < width - 1)
	{
		position.first++;
	}
	else
	{
		position.first = 0;
	}

	rgb_t color;
	provinceMap.get_pixel(position.first, position.second, color);

	ConverterColor::Color theColor(color.red, color.green, color.blue);
	return theColor;
}


void provinceNeighborMapper::handleNeighbor(ConverterColor::Color centerColor, ConverterColor::Color otherColor, const point& position)
{
	auto centerProvince = provinceDefinitions::getProvinceFromColor(centerColor);
	auto otherProvince = provinceDefinitions::getProvinceFromColor(otherColor);

	if (centerProvince && otherProvince)
	{
		addNeighbor(*centerProvince, *otherProvince);
		addPointToBorder(*centerProvince, *otherProvince, position);
	}
}


void provinceNeighborMapper::addNeighbor(int mainProvince, int neighborProvince)
{
	auto centerMapping = provinceNeighbors.find(mainProvince);
	if (centerMapping != provinceNeighbors.end())
	{
		centerMapping->second.insert(neighborProvince);
	}
	else
	{
		set<int> neighbors = { neighborProvince };
		provinceNeighbors[mainProvince] = neighbors;
	}
}


void provinceNeighborMapper::addPointToBorder(int mainProvince, int neighborProvince, point position)
{
	auto bordersWithNeighbors = borders.find(mainProvince);
	if (bordersWithNeighbors == borders.end())
	{
		bordersWith newBordersWithNeighbors;
		borders.insert(make_pair(mainProvince, newBordersWithNeighbors));
		bordersWithNeighbors = borders.find(mainProvince);
	}

	auto border = bordersWithNeighbors->second.find(neighborProvince);
	if (border == bordersWithNeighbors->second.end())
	{
		borderPoints newBorder;
		bordersWithNeighbors->second.insert(make_pair(neighborProvince, newBorder));
		border = bordersWithNeighbors->second.find(neighborProvince);
	}

	if (border->second.size() == 0)
	{
		border->second.push_back(position);
	}
	else
	{
		auto lastPoint = border->second.back();
		if ((lastPoint.first != position.first) || (lastPoint.second != position.second))
		{
			border->second.push_back(position);
		}
	}
}


const set<int> provinceNeighborMapper::GetNeighbors(int province) const
{
	auto neighbors = provinceNeighbors.find(province);
	if (neighbors != provinceNeighbors.end())
	{
		return neighbors->second;
	}
	else
	{
		set<int> empty;
		return empty;
	}
}


const optional<point> provinceNeighborMapper::GetBorderCenter(int mainProvince, int neighbor) const
{
	auto bordersWithNeighbors = borders.find(mainProvince);
	if (bordersWithNeighbors == borders.end())
	{
		LOG(LogLevel::Warning) << "Province " << mainProvince << " has no borders.";
		return make_pair(-1, -1);
	}
	auto border = bordersWithNeighbors->second.find(neighbor);
	if (border == bordersWithNeighbors->second.end())
	{
		LOG(LogLevel::Warning) << "Province " << mainProvince << " does not border " << neighbor << ".";
		return make_pair(-1, -1);
	}

	return border->second[(border->second.size() / 2)];
}


optional<int> provinceNeighborMapper::GetProvinceNumber(double x, double y)
{
	rgb_t color;
	provinceMap.get_pixel(static_cast<unsigned int>(x), (provinceMap.height() - 1) - static_cast<unsigned int>(y), color);
	ConverterColor::Color theColor(color.red, color.green, color.blue);
	return provinceDefinitions::getProvinceFromColor(theColor);
}