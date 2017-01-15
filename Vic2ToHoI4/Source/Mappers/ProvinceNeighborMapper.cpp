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
#include "provinceDefinitions.h"
#include "../Color.h"
#include "../Configuration.h"
#include "log.h"



provinceNeighborMapper* provinceNeighborMapper::instance = nullptr;



provinceNeighborMapper::provinceNeighborMapper()
{
	bitmap_image provinces(Configuration::getHoI4Path() + "/map/provinces.bmp");
	if (!provinces)
	{
		LOG(LogLevel::Error) << "Could not open " << Configuration::getHoI4Path() << "/map/provinces.bmp";
	}

	unsigned int height = provinces.height();
	unsigned int width = provinces.width();

	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			Color centerColor = getCenterColor(provinces, x, y);
			Color aboveColor = getAboveColor(provinces, x, y, height);
			Color belowColor = getBelowColor(provinces, x, y, height);
			Color leftColor = getLeftColor(provinces, x, y, width);
			Color rightColor = getRightColor(provinces, x, y, width);

			if (centerColor != aboveColor)
			{
				handleNeighbor(centerColor, aboveColor);
			}
			if (centerColor != belowColor)
			{
				handleNeighbor(centerColor, belowColor);
			}
			if (centerColor != leftColor)
			{
				handleNeighbor(centerColor, leftColor);
			}
			if (centerColor != rightColor)
			{
				handleNeighbor(centerColor, rightColor);
			}
		}
	}
}


Color provinceNeighborMapper::getCenterColor(bitmap_image& provinces, int x, int y)
{
	rgb_t color;
	provinces.get_pixel(x, y, color);

	Color theColor(color.red, color.green, color.blue);
	return theColor;
}


Color provinceNeighborMapper::getAboveColor(bitmap_image& provinces, int x, int y, int height)
{
	if (y > 0)
	{
		y--;
	}

	rgb_t color;
	provinces.get_pixel(x, y, color);

	Color theColor(color.red, color.green, color.blue);
	return theColor;
}


Color provinceNeighborMapper::getBelowColor(bitmap_image& provinces, int x, int y, int height)
{
	if (y < height - 1)
	{
		y++;
	}

	rgb_t color;
	provinces.get_pixel(x, y, color);

	Color theColor(color.red, color.green, color.blue);
	return theColor;
}


Color provinceNeighborMapper::getLeftColor(bitmap_image& provinces, int x, int y, int width)
{
	if (x > 0)
	{
		x--;
	}
	else
	{
		x = width - 1;
	}

	rgb_t color;
	provinces.get_pixel(x, y, color);

	Color theColor(color.red, color.green, color.blue);
	return theColor;
}


Color provinceNeighborMapper::getRightColor(bitmap_image& provinces, int x, int y, int width)
{
	if (x < width - 1)
	{
		x++;
	}
	else
	{
		x = 0;
	}

	rgb_t color;
	provinces.get_pixel(x, y, color);

	Color theColor(color.red, color.green, color.blue);
	return theColor;
}


void provinceNeighborMapper::handleNeighbor(Color centerColor, Color otherColor)
{
	int centerProvince = provinceDefinitions::getProvinceFromColor(centerColor);
	int otherProvince = provinceDefinitions::getProvinceFromColor(otherColor);

	auto centerMapping = provinceNeighbors.find(centerProvince);
	if (centerMapping != provinceNeighbors.end())
	{
		centerMapping->second.insert(otherProvince);
	}
	else
	{
		set<int> neighbors = { otherProvince };
		provinceNeighbors[centerProvince] = neighbors;
	}
}


const set<int> provinceNeighborMapper::GetNeighbors(int province)
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