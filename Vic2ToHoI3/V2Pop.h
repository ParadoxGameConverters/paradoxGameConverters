#ifndef V2POP_H_
#define V2POP_H_

#include "Parsers/Object.h"

class V2Pop
{
	public:
		void					init(Object* obj);
		string					getType() const { return type; };
		int						getId() const { return id; };
		int						getSize() const { return size; };

	private:
		string					type;
		int						id;
		int						size;
		double					money;
		double					mil;
		double					con;
		double					lit;
};

#endif