/*Copyright (c) 2016 The Paradox Game Converters Project

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



#include "Color.h"

#include <chrono>
#include <random>

#include <boost/lexical_cast.hpp>

#include "Parsers/Object.h"

Color::Color()
: initialized(false), c({ 0, 0, 0 })
{}

Color::Color(const int r, const int g, const int b)
: initialized(true), c({ r, g, b })
{}

Color::Color(Object* colorObject)
: initialized(false), c({ 0, 0, 0 })
{
	auto colorTokens = colorObject->getTokens();	// the colors held by the object
	initialized = (colorTokens.size() >= 3);
	for (size_t i = 0; i < 3; ++i)
	{
		if (!colorTokens[i].empty())
		{
			c[i] = boost::lexical_cast<int>(colorTokens[i]);
		}
	}
}

void Color::RandomlyFlunctuate(const int stdDev)
{
	// All three color components will go up or down by the some amount (according to stdDev), 
	// and then each is tweaked a bit more (with a much smaller standard deviation).
	static std::mt19937 generator(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
	const double allChange = std::normal_distribution<double>(0.0, stdDev)(generator);	// the amount the colors all change by
	std::normal_distribution<double> distribution(0.0, stdDev / 4.0);
	for (auto& component : c)	// the component under consideration
	{
		component += static_cast<int>(allChange + distribution(generator) + 0.5);
		if (component < 0)
		{
			component = 0;
		}
		else if (component > 255)
		{
			component = 255;
		}
	}
}

std::ostream& operator<<(std::ostream& out, const Color& color)
{
	out << color.c[0] << ' ' << color.c[1] << ' ' << color.c[2];
	return out;
}

void Color::GetRGB(int& r, int& g, int& b) const
{
	r = c[0];
	g = c[1];
	b = c[2];
}

Color::operator bool() const
{
	return initialized;
}
