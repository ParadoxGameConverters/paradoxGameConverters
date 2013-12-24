#ifndef EU4LOAN_H_
#define EU4LOAN_H_


#include <string>
using namespace std;

class Object;



class EU4Loan
{
	public:
		EU4Loan(Object* obj);
		string	getLender() const { return lender; };
		double	getInterest() const { return interest; };
		double	getAmount() const { return amount; };
	private:
		string	lender;
		double	interest;
		double	amount;
};



#endif // EU4LOAN_H_