#ifndef EU3PROVINCE_H_
#define EU3PROVINCE_H_


#include <string>
#include <vector>
#include "..\Date.h"
using namespace std;



class Object;
class EU3History;
class EU3Ruler;
class EU3Advisor;

class EU3Province
{
	public:
		void		init(int num, Object* obj, date startDate);
		void		output(FILE*);
		void		setNumber(int);
		void		addCore(string);
		void		setOwner(string);
		void		setInHRE(bool);
		void		setDiscoveredBy(vector<string>);
		void		addAdvisor(EU3Advisor*);
		string	getOwner();
	private:
		int							num;
		string						owner;
		vector<string>				cores;
		bool							inHRE;
		vector<string>				discoveredBy;
		vector<EU3History*>		history;
};



#endif	// EU3PROVINCE_H_