#ifndef V2Province_H_
#define V2Province_H_

#include "V2Province.h"
#include <vector>
#include "tempFuncs.h"
using namespace std;


class V2State
{
	public:
		V2State(int newId);
		void	addProvince(V2Province);
		void	setColonial(bool);
		void	output(FILE*);
	private:
		int						id;
		bool						colonial;
		vector<V2Province>	provinces;
};


#endif	// V2Province_H_