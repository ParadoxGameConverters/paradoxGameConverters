#include "Color.h"

#include <boost/lexical_cast.hpp>

#include "Parsers/Object.h"

Color::Color()
: initialized(false), r(0), g(0), b(0)
{}

Color::Color(int r, int g, int b)
: initialized(true), r(r), g(g), b(b)
{}

Color::Color(Object* colorObject)
: initialized(false), r(0), g(0), b(0)
{
	auto colorTokens = colorObject->getTokens();
	initialized = (colorTokens.size() >= 3);
	if (!colorTokens[0].empty())
	{
		r = boost::lexical_cast<int>(colorTokens[0]);
	}
	if (!colorTokens[1].empty())
	{
		g = boost::lexical_cast<int>(colorTokens[1]);
	}
	if (!colorTokens[2].empty())
	{
		b = boost::lexical_cast<int>(colorTokens[2]);
	}
}

std::ostream& operator<<(std::ostream& out, const Color& color)
{
	out << color.r << ' ' << color.g << ' ' << color.b;
}

void Color::GetRGB(int& rOut, int& gOut, int& bOut) const
{
	rOut = r;
	gOut = g;
	bOut = b;
}

Color::operator bool() const
{
	return initialized;
}
