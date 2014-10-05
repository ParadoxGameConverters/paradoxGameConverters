#include "Color.h"

#include <chrono>
#include <random>

#include <boost/lexical_cast.hpp>

#include "Parsers/Object.h"

Color::Color()
: initialized(false), c({ 0, 0, 0 })
{}

Color::Color(int r, int g, int b)
: initialized(true), c({ r, g, b })
{}

Color::Color(Object* colorObject)
: initialized(false), c({ 0, 0, 0 })
{
	auto colorTokens = colorObject->getTokens();
	initialized = (colorTokens.size() >= 3);
	for (size_t i = 0; i < 3; ++i)
	{
		if (!colorTokens[i].empty())
		{
			c[i] = boost::lexical_cast<int>(colorTokens[i]);
		}
	}
}

void Color::RandomlyFlunctuate(int stdDev)
{
	// All three color components will go up or down by the some amount (according to stdDev), 
	// and then each is tweaked a bit more (with a much smaller standard deviation).
	static std::mt19937 generator(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
	double allChange = std::normal_distribution<double>(0.0, stdDev)(generator);
	std::normal_distribution<double> distribution(0.0, stdDev / 4.0);
	for (auto& component : c)
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
