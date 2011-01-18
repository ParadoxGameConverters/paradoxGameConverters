#include <string>
#include "Parsers\Object.h"
using namespace std;


class V2Country
{
	public:
		void init(Object* obj);
		string getTag();
	private:
		string tag;
};