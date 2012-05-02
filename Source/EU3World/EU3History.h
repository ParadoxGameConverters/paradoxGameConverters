#ifndef EU3HISTORY_H_
#define EU3HISTORY_H_


#include "..\Date.h"
#include <fstream>


class CK2History;
class EU3Ruler;

class EU3History {
	public:
		void			init(CK2History*);
		void			initHeir(EU3Ruler*);
		void			output(FILE*);
		EU3Ruler*	getMonarch();
	private:
		date			when;
		EU3Ruler*	monarch;
		EU3Ruler*	heir;
};


#endif // EU3HISTORY_H_