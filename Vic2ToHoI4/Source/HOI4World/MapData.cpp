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



#include "MapData.h"
#include "../Mappers/ProvinceDefinitions.h"
#include "../Color.h"
#include "../Configuration.h"
#include "Log.h"



void HoI4::provincePoints::addPoint(const point& thePoint)
{
	thePoints.emplace(thePoint);
	if (thePoint.first < leftmostPoint.first)
	{
		leftmostPoint = thePoint;
	}
	if (thePoint.first > rightmostPoint.first)
	{
		rightmostPoint = thePoint;
	}
	if (thePoint.second < lowestPoint.second)
	{
		lowestPoint = thePoint;
	}
	if (thePoint.second > highestPoint.second)
	{
		highestPoint = thePoint;
	}
}


point HoI4::provincePoints::getCentermostPoint()
{
	point possibleCenter;
	possibleCenter.first = (leftmostPoint.first + rightmostPoint.first) / 2;
	possibleCenter.second = (lowestPoint.second + highestPoint.second) / 2;
	if (thePoints.count(possibleCenter) > 0)
	{
		return possibleCenter;
	}
	else
	{
		float shortestDistance = 1000000;
		point closestPoint;
		for (auto possiblePoint: thePoints)
		{
			float distanceSquared;
			float deltaX = static_cast<float>(possiblePoint.first - possibleCenter.first);
			distanceSquared = deltaX * deltaX;
			float deltaY = static_cast<float>(possiblePoint.second - possibleCenter.second);
			distanceSquared += deltaY * deltaY;

			if (distanceSquared < shortestDistance)
			{
				shortestDistance = distanceSquared;
				closestPoint = possiblePoint;
			}
		}
		return closestPoint;
	}
}


HoI4::MapData::MapData() noexcept:
	provinceMap(theConfiguration.getHoI4Path() + "/map/provinces.bmp")
{
	if (!provinceMap)
	{
		LOG(LogLevel::Error) << "Could not open " << theConfiguration.getHoI4Path() << "/map/provinces.bmp";
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

			auto province = provinceDefinitions::getProvinceFromColor(centerColor);
			if (province)
			{
				auto specificProvincePoints = theProvincePoints.find(*province);
				if (specificProvincePoints != theProvincePoints.end())
				{
					specificProvincePoints->second.addPoint(position);
				}
				else
				{
					provincePoints theNewPoints;
					theNewPoints.addPoint(position);
					theProvincePoints.insert(make_pair(*province, theNewPoints));
				}
			}
		}
	}
}


ConverterColor::Color HoI4::MapData::getCenterColor(point position)
{
	rgb_t color;
	provinceMap.get_pixel(position.first, position.second, color);

	ConverterColor::Color theColor(ConverterColor::red(color.red), ConverterColor::green(color.green), ConverterColor::blue(color.blue));
	return theColor;
}


ConverterColor::Color HoI4::MapData::getAboveColor(point position, int height)
{
	if (position.second > 0)
	{
		position.second--;
	}

	rgb_t color;
	provinceMap.get_pixel(position.first, position.second, color);

	ConverterColor::Color theColor(ConverterColor::red(color.red), ConverterColor::green(color.green), ConverterColor::blue(color.blue));
	return theColor;
}


ConverterColor::Color HoI4::MapData::getBelowColor(point position, int height)
{
	if (position.second < height - 1)
	{
		position.second++;
	}

	rgb_t color;
	provinceMap.get_pixel(position.first, position.second, color);

	ConverterColor::Color theColor(ConverterColor::red(color.red), ConverterColor::green(color.green), ConverterColor::blue(color.blue));
	return theColor;
}


ConverterColor::Color HoI4::MapData::getLeftColor(point position, int width)
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

	ConverterColor::Color theColor(ConverterColor::red(color.red), ConverterColor::green(color.green), ConverterColor::blue(color.blue));
	return theColor;
}


ConverterColor::Color HoI4::MapData::getRightColor(point position, int width)
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

	ConverterColor::Color theColor(ConverterColor::red(color.red), ConverterColor::green(color.green), ConverterColor::blue(color.blue));
	return theColor;
}


void HoI4::MapData::handleNeighbor(ConverterColor::Color centerColor, ConverterColor::Color otherColor, const point& position)
{
	auto centerProvince = provinceDefinitions::getProvinceFromColor(centerColor);
	auto otherProvince = provinceDefinitions::getProvinceFromColor(otherColor);

	if (centerProvince && otherProvince)
	{
		addNeighbor(*centerProvince, *otherProvince);
		addPointToBorder(*centerProvince, *otherProvince, position);
	}
}


void HoI4::MapData::addNeighbor(int mainProvince, int neighborProvince)
{
	auto centerMapping = provinceNeighbors.find(mainProvince);
	if (centerMapping != provinceNeighbors.end())
	{
		centerMapping->second.insert(neighborProvince);
	}
	else
	{
		std::set<int> neighbors = { neighborProvince };
		provinceNeighbors[mainProvince] = neighbors;
	}
}


void HoI4::MapData::addPointToBorder(int mainProvince, int neighborProvince, point position)
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


set<int> HoI4::MapData::getNeighbors(int province) const
{
	auto neighbors = provinceNeighbors.find(province);
	if (neighbors != provinceNeighbors.end())
	{
		return neighbors->second;
	}
	else
	{
		std::set<int> empty;
		return empty;
	}
}


optional<point> HoI4::MapData::getSpecifiedBorderCenter(int mainProvince, int neighbor) const
{
	auto bordersWithNeighbors = borders.find(mainProvince);
	if (bordersWithNeighbors == borders.end())
	{
		LOG(LogLevel::Warning) << "Province " << mainProvince << " has no borders.";
		return std::nullopt;
	}
	auto border = bordersWithNeighbors->second.find(neighbor);
	if (border == bordersWithNeighbors->second.end())
	{
		LOG(LogLevel::Warning) << "Province " << mainProvince << " does not border " << neighbor << ".";
		return std::nullopt;
	}

	return border->second[(border->second.size() / 2)];
}


optional<point> HoI4::MapData::getAnyBorderCenter(int province) const
{
	auto bordersWithNeighbors = borders.find(province);
	if (bordersWithNeighbors == borders.end())
	{
		LOG(LogLevel::Warning) << "Province " << province << " has no borders.";
		return std::nullopt;
	}
	auto border = bordersWithNeighbors->second.begin();
	if (border == bordersWithNeighbors->second.end())
	{
		LOG(LogLevel::Warning) << "Province " << province << " has no borders.";
		return std::nullopt;
	}

	return border->second[(border->second.size() / 2)];
}


optional<int> HoI4::MapData::getProvinceNumber(double x, double y)
{
	rgb_t color;
	provinceMap.get_pixel(static_cast<unsigned int>(x), (provinceMap.height() - 1) - static_cast<unsigned int>(y), color);
	ConverterColor::Color theColor(ConverterColor::red(color.red), ConverterColor::green(color.green), ConverterColor::blue(color.blue));
	return provinceDefinitions::getProvinceFromColor(theColor);
}


std::optional<HoI4::provincePoints> HoI4::MapData::getProvincePoints(int provinceNum) const
{
	auto possiblePoints = theProvincePoints.find(provinceNum);
	if (possiblePoints != theProvincePoints.end())
	{
		return possiblePoints->second;
	}
	else
	{
		return std::nullopt;
	}
}