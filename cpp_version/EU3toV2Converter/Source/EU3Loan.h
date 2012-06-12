#ifndef EU3LOAN_H_
#define EU3LOAN_H_


#include <string>
using namespace std;

class Object;



class EU3Loan
{
	public:
		EU3Loan();
		void		init(Object* obj);
		string	getLender() const { return lender; };
		double	getInterest() const { return interest; };
		double	getAmount() const { return amount; };
	private:
		string	lender;
		double	interest;
		double	amount;
};



#endif // EU3LOAN_H_