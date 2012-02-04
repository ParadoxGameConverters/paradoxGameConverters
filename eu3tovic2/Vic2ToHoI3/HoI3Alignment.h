#ifndef HOI3ALIGNMENT_H_
#define HOI3ALIGNMENT_H_

#include <math.h>
#include <stdio.h>

/*
2D Alignment:
		   X+0    <- Allies
		   Y+146
            |
Comintern   |       Axis
   |        |        |
   v	    |        v
X-200	    |      X+200
Y-200______________Y-200

Circumcenter (true neutral) is at:					(0, -85)
Distance from any corner to circumcenter is:		230
Distance from any corner to the opposite side is:	346

In the save, Y is flipped.
*/

class HoI3Alignment
{
public:
	HoI3Alignment()							: X(0.0), Y(-85.0)					{}; // constructed neutral
	void	output(FILE* out);

	// 2D alignments are as per above
	double	get2DX() const					{ return X; };
	double	get2DY() const					{ return Y; };
	void	set2D(double _x, double _y)		{ X = _x; Y = _y; };

	// Linear alignments.  Distances are 0 to 346.  Neutral is 230.
	double	getAlliedDistance() const		{ return sqrt( pow(0.0-X, 2) + pow(146.0-Y, 2) ); };
	double	getAxisDistance() const			{ return sqrt( pow(200.0-X, 2) + pow(-200.0-Y, 2) ); };
	double	getCominternDistance() const	{ return sqrt( pow(-200.0-X, 2) + pow(-200.0-Y, 2) ); };

	void	moveTowardsAllied(double amt)	{ X += (   0.0 - X) / getAlliedDistance() * amt;
											  Y += ( 146.0 - Y) / getAlliedDistance() * amt; };
	void	moveTowardsAxis(double amt)		{ X += ( 200.0 - X) / getAxisDistance() * amt;
											  Y += (-200.0 - Y) / getAxisDistance() * amt; };
	void	moveTowardsComintern(double amt){ X += (-200.0 - X) / getCominternDistance() * amt;
											  Y += (-200.0 - Y) / getCominternDistance() * amt; };

	// Moves to the corners
	void	alignToAllied()					{ X =    0.0;	Y =  146.0; }
	void	alignToAxis()					{ X =  200.0;	Y = -200.0; }
	void	alignToComintern()				{ X = -200.0;	Y = -200.0; }

	// Find centroid (mean) of three alignments (used for relations-weighting)
	static HoI3Alignment getCentroid(const HoI3Alignment& A, const HoI3Alignment& B, const HoI3Alignment& C);

private:
	double	X;
	double	Y;
};

#endif // HOI3ALIGNMENT_H_