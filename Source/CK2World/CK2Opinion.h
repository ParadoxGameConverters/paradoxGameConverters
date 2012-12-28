#ifndef CK2OPINION_H_
#define CK2OPINION_H_

#include <map>
#include <string>
using namespace std;

class Object;

class CK2Opinion
{
	public:
		CK2Opinion() : value(0), multiplier(1) {};
		CK2Opinion(Object*);

		int getTotalOpinion() const { return multiplier * value; }

		static void initOpinions(Object* root);
		static int getBaseValue(string opinion);

	private:
		int multiplier;
		int value;

		static map<string, int> opinionVals;
};

#endif // CK2OPINION_H_