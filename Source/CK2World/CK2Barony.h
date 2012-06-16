#ifndef CK2BARONY_H_
#define CK2BARONY_H_



class Object;
class CK2Title;
class CK2Province;

class CK2Barony
{
	public:
		CK2Barony();
		void				init(Object*, CK2Title*, CK2Province*);
		CK2Title*		getTitle();
		CK2Province*	getProvince();
	private:
		CK2Title*		title;
		CK2Province*	province;
};



#endif // CK2BARONY_H_