#include <string>
#include <vector>
using namespace std;


class EU3Country
{
	public:
		void		init(string tag, string countryFile);
		string	getTag();
	private:
		string	tag;
		string	countryFile;
};