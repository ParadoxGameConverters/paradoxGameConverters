#ifndef EU3LOAN_H_
#define EU3LOAN_H_

#include "Parsers\Object.h"

class EU3Loan
{
public:
	void	init(Object* obj);
	string	getLender() const { return lender; };
	double	getInterest() const { return interest; };
	double	getAmount() const { return amount; };

private:
	string	lender;
	double	interest;
	double	amount;
};

#endif // EU3LOAN_H_