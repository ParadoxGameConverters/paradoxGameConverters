#ifndef COLOR_H_
#define COLOR_H_

#include <iostream>

class Object;

// An RGB color triplet.
class Color
{
public:
	// Default initializes the color values to 0.
	Color();
	// Initializes the color with a given RGB color triplet.
	Color(int r, int g, int b);
	// Initializes the color from an object node whose leaf value is
	// an RGB color triplet (separated only by whitespace).
	Color(Object* colorObject);

	// Writes the RGB triplet to the stream as "R G B".
	friend std::ostream& operator<<(std::ostream&, const Color&);

	// Passes back the RGB color triplet as individual components.
	void GetRGB(int& r, int& g, int& b) const;

	// Returns true if the color has been initialized with an RGB triplet.
	operator bool() const;

private:
	bool initialized;
	int r;
	int g;
	int b;
};

#endif