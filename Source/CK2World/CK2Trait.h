#ifndef CK2TRAIT_H_
#define CK2TRAIT_H_



class Object;

class CK2Trait
{
	public:
		CK2Trait();
		void	init(Object*);

		int	diplomacy;
		int	martial;
		int	stewardship;
		int	intrigue;
		int	learning;
};



#endif // CK2TRAIT_H_