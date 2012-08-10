#ifndef EU3HISTORY_H_
#define EU3HISTORY_H_


#include "..\Date.h"
#include "..\Mappers.h"
#include <fstream>


class CK2History;
class EU3Ruler;
class EU3Advisor;

class EU3History {
	public:
		EU3History();
		EU3History(CK2History*);
		EU3History(date when, EU3Ruler* monarch, EU3Ruler* regent, EU3Ruler* heir, EU3Advisor* advisor, string capital, string owner, string culture, string religion, vector<string> discoverers);

		void				output(FILE*);

		date				getWhen()		const { return when; };
		EU3Ruler*		getMonarch()	const { return monarch; };
		EU3Ruler*		getRegent()		const { return regent ; };
		EU3Ruler*		getHeir()		const { return heir; };

	private:
		date				when;
		EU3Ruler*		monarch;
		EU3Ruler*		regent;
		EU3Ruler*		heir;
		EU3Advisor*		advisor;
		string			capital;
		string			owner;
		string			culture;
		string			religion;
		vector<string>	discoverers;
};


#endif // EU3HISTORY_H_