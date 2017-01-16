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



#ifndef PROVINCE_NEIGHBOR_MAPPER_H
#define PROVINCE_NEIGHBOR_MAPPER_H



#include <map>
#include <set>
#include "../bitmap_image.hpp"
#include "../Color.h"
using namespace std;



typedef pair<int, int> point;
typedef vector<point> borderPoints;
typedef map<int, borderPoints> bordersWith;



class provinceNeighborMapper
{
	public:
		static const set<int> getNeighbors(int province)
		{
			return getInstance()->GetNeighbors(province);
		}

		static const point getBorderCenter(int mainProvince, int neighbor)
		{
			return getInstance()->GetBorderCenter(mainProvince, neighbor);
		}

	private:
		static provinceNeighborMapper* instance;
		static provinceNeighborMapper* getInstance()
		{
			if (instance == nullptr)
			{
				instance = new provinceNeighborMapper();
			}

			return instance;
		}
		provinceNeighborMapper();

		Color getCenterColor(bitmap_image& provinces, point position);
		Color getAboveColor(bitmap_image& provinces, point position, int height);
		Color getBelowColor(bitmap_image& provinces, point position, int height);
		Color getLeftColor(bitmap_image& provinces, point position, int width);
		Color getRightColor(bitmap_image& provinces, point position, int width);
		void handleNeighbor(Color centerColor, Color otherColor, point position);
		void addNeighbor(int mainProvince, int neighborProvince);
		void addPointToBorder(int mainProvince, int neighborProvince, point position);

		const set<int> GetNeighbors(int province);
		const point GetBorderCenter(int mainProvince, int neighbor);

		map<int, set<int>> provinceNeighbors;
		map<int, bordersWith> borders;
};



#endif //PROVINCE_NEIGHBOR_MAPPER_H
