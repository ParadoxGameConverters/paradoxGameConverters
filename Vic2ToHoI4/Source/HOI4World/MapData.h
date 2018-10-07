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



#ifndef MAP_DATA_H
#define MAP_DATA_H



#include <optional>
#include <map>
#include <set>
#include "../bitmap_image.hpp"
#include "../Color.h"



typedef std::pair<int, int> point;
typedef std::vector<point> borderPoints;
typedef std::map<int, borderPoints> bordersWith;



namespace HoI4
{

class provincePoints
{
	public:
		provincePoints() = default;

		void addPoint(const point& thePoint);

		point getCentermostPoint();

	private:
		std::set<point> thePoints;
		point leftmostPoint = {1000000, 0};
		point rightmostPoint = {-1, 0};
		point highestPoint = {0, -1};
		point lowestPoint = {0, 1000000};
};


class MapData
{
	public:
		MapData() noexcept;

		std::set<int> getNeighbors(int province) const;
		std::optional<point> getSpecifiedBorderCenter(int mainProvince, int neighbor) const;
		std::optional<point> getAnyBorderCenter(int province) const;
		std::optional<int> getProvinceNumber(double x, double y);

		std::optional<provincePoints> getProvincePoints(int provinceNum) const;

	private:
		MapData(const MapData&) = delete;
		MapData& operator=(const MapData&) = delete;

		ConverterColor::Color getCenterColor(point position);
		ConverterColor::Color getAboveColor(point position, int height);
		ConverterColor::Color getBelowColor(point position, int height);
		ConverterColor::Color getLeftColor(point position, int width);
		ConverterColor::Color getRightColor(point position, int width);
		void handleNeighbor(ConverterColor::Color centerColor, ConverterColor::Color otherColor, const point& position);
		void addNeighbor(int mainProvince, int neighborProvince);
		void addPointToBorder(int mainProvince, int neighborProvince, point position);

		std::map<int, std::set<int>> provinceNeighbors;
		std::map<int, bordersWith> borders;
		std::map<int, provincePoints> theProvincePoints;

		bitmap_image provinceMap;
};

}



#endif //MAP_DATA_H
