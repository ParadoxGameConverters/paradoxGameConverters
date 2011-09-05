#ifndef V2POP_H_
#define V2POP_H_


#include <string>
using namespace std;


class V2Pop
{
	public:
		V2Pop();
		void		setType(string);
		void		setSize(int);
		void		setCulture(string);
		void		setReligion(string);
		int		getSize() const;
		string	getType() const;
		void		output(FILE*);
		int		getID() const;
		string	getCulture() const;
		string	getReligion() const;
		int		getSupportedRegimentCount() const;
		void	setSupportedRegimentCount(int regiments);
		bool	canCombine(const V2Pop& rhs);

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