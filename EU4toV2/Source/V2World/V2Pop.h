#ifndef V2POP_H_
#define V2POP_H_



#include <string>
#include <vector>
using namespace std;



class V2Pop
{
	public:
		V2Pop(string type, int size, string culture, string religion);
		void output(FILE*) const;
		bool combine(const V2Pop& rhs);

		void	changeSize(int delta)					{ size += delta; recalcMoney(); };
		void	incrementSupportedRegimentCount()	{ supportedRegiments++; };

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
};


int getNextPopId();



#endif