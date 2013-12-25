#include "HoI3Alignment.h"

#include "Log.h"


void HoI3Alignment::output(FILE* out)
{
	// Valid space defined by: Y >= -200, Y <= 2X + 200, Y <= -2X + 200
	if (Y < -200.0)				log("Alignment out of bounds (bottom): %f %f\n", X, Y);
	if (Y > 2.0 * X + 200.0)	log("Alignment out of bounds (top-left): %f %f\n", X, Y);
	if (Y > -2.0 * X + 200.0)	log("Alignment out of bounds (top-right): %f %f\n", X, Y);

	fprintf(out, "\talignment=\n");
	fprintf(out, "\t{\n");
	fprintf(out, "\t\tposition=\n");
	fprintf(out, "\t\t{\n");
	fprintf(out, "\t\t\tx=%f\n", X);
	fprintf(out, "\t\t\ty=%f\n", -Y); // whoops, reversed Y
	fprintf(out, "\t\t}\n");
	fprintf(out, "\t}\n");
}


HoI3Alignment HoI3Alignment::getCentroid(const HoI3Alignment &A, const HoI3Alignment &B, const HoI3Alignment &C)
{
	HoI3Alignment retval; // default constructor = true neutral

	double _x = (A.get2DX() + B.get2DX() + C.get2DX()) / 3.0;
	double _y = (A.get2DY() + B.get2DY() + C.get2DY()) / 3.0;
	retval.set2D(_x, _y);
	
	return retval;
}