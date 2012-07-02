#ifndef V2CREDITOR_H_
#define V2CREDITOR_H_



#include <string>
using namespace std;



class V2Creditor
{
	public:
		V2Creditor(string _country) : country(_country), interest(0.0), debt(0.0) {};
		void output(FILE* output);
		void addLoan(double size, double _interest);
	private:
		string	country;
		double	interest;
		double	debt;
};



#endif // V2CREDITOR_H_