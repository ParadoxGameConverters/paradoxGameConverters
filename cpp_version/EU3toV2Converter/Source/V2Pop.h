#ifndef V2POP_H_
#define V2POP_H_



#include <string>
#include <vector>
using namespace std;



class V2Pop
{
	public:
		V2Pop(string type, int size, string culture, string religion, double literacy, double reactionary, double conservative, double liberal, vector< pair<int, double> > issues);
		void output(FILE*);
		bool combine(const V2Pop& rhs);

		void	changeSize(int delta)					{ size += delta; recalcMoney(); };
		void	incrementSupportedRegimentCount()	{ supportedRegiments++; };
		void	setConsciousness(double con)			{ consciousness = con; };
		void	setMilitancy(double mil)				{ militancy = mil; };

		int		getSize()							const	{ return size; };
		string	getType()							const	{ return type; };
		int		getID()								const	{ return id; };
		string	getCulture()						const	{ return culture; };
		string	getReligion()						const	{ return religion; };
		int		getSupportedRegimentCount()	const	{ return supportedRegiments; };
	private:
		void	recalcMoney();

		int		id;
		string	type;
		int		size;
		string	culture;
		string	religion;
		int		supportedRegiments;
		double	money;
		double	consciousness;
		double	literacy;
		double	militancy;
		double	reactionary;
		double	conservative;
		double	liberal;
		vector< pair<int, double> >	issues;
};


int getNextPopId();



#endif