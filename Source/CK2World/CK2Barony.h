#ifndef CK2BARONY_H_
#define CK2BARONY_H_



class Object;
class CK2Title;

class CK2Barony
{
	public:
		void			init(Object*, CK2Title*);
		CK2Title*	getTitle();
	private:
		CK2Title*	title;
};



#endif // CK2BARONY_H_