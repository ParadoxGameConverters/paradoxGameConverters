#include "Parser.h"
#include <fstream>

Object* Parser::topLevel = NULL;  
vector<Object*> stack; 
vector<Object*> objstack; 

void initParser()
{
	Parser::topLevel = new Object("topLevel");
}

bool trim(string& str, int& count)	// Returns net number of braces opened by str, and trims leading and trailing whitespace.
{
	const char*		s			= str.c_str();
	unsigned int	strSize	= str.size();
	bool				opened	= false;

	if (0 == strSize)
	{
		return 0;
	}

	bool isInLiteral = false;
	for (unsigned int i = 0; i < strSize; ++i)
	{
		if ('"' == s[i])
		{
			isInLiteral = !isInLiteral;
		}
		if (isInLiteral)
		{
			continue;
		}
		if ('#' == s[i])
		{
			break;
		}
		if ('{' == s[i])
		{
			++count;
			opened = true;
		}
		else if ('}' == s[i])
		{
			--count;
		}
	}

	unsigned int first = 0;
	for (; first < strSize; ++first)
	{
		if (s[first] == ' ')
		{
			continue;
		}
		if ('\r' == s[first]) // Carriage return
		{
			continue;
		}
		break;
	}

	unsigned int last = strSize - 1; 
	for (; last > first; --last)
	{
		if (' ' == s[last])
		{
			continue;
		}
		if ('\r' == (int) s[last]) // Carriage return
		{
			continue; 
		}
		break;
	}

	unsigned int fcomment = 0;
	for (; fcomment < strSize; ++fcomment)
	{
		if (s[fcomment] == '\"')
			isInLiteral = !isInLiteral;
		if (!isInLiteral && (s[fcomment] == '#'))
			break;
	}
	if (fcomment < last)
	{
		last = fcomment - 1;
	}

	str = str.substr(first, last - first + 1); 
	return opened; 
}

void readFile (ifstream& read)
{
	clearStack();

	int openBraces = 0;
	string currObject;
	bool topLevel = true;
	while (read.good())
	{
		string buffer;
		getline(read, buffer);
		for (string::iterator i = buffer.begin(); i != buffer.end(); i++)
		{
			if (*i == '\r')
			{
				buffer.erase(i);
				i--;
			}
		}

		bool opened = trim(buffer, openBraces);
		currObject		+= " " + buffer;

		if (openBraces > 0)
		{
			topLevel = false;
			continue;
		}

		if (currObject == "")
		{
			continue;
		}

		// Don't try to end an object that hasn't started properly;
		// accounts for such constructions as
		// object = 
		// { 
		// where openBraces is zero after the first line of the object. 
		// Not a problem for non-top-level objects since these will have
		// nonzero openBraces anyway.
		// But don't continue if the object was all on one line.
		if (topLevel && !opened)
		{
			continue; 
		}

		openBraces = 0; 
		makeObject(currObject);
		currObject.clear(); 
		topLevel = true; 
	}

	trim(currObject, openBraces); 
	// MAX: Ignore this 
	/*
	if (currObject.size() > 0) {
		cout << "Warning: Unable to parse file; problem is with \"" << currObject << "\"" << endl; 
		cout << "Aborting, unable to parse file" << endl; 
		abort();
	}
	*/
}


void clearStack()
{
	if (0 < stack.size())
	{
		cout << "Warning: Clearing stack size " << stack.size() << ". This should not happen in normal operation." << endl;
	}
	for (vector<Object*>::iterator i = stack.begin(); i != stack.end(); ++i)
	{
		//cout << (*i)->getKey() << endl;
		cout << (*(*i)) << endl;
	}
	stack.clear();
}


void setLHS(char const* first, char const* last)
{
	string key(first, last);
	//cout << "Setting lefthand side " << key << endl;
	Object* p = new Object(key);
	if (0 == stack.size())
	{
		Parser::topLevel->setValue(p);
	}
	stack.push_back(p);
}

void pushObj(char const* first, char const* last)
{
	string key("objlist");
	Object* p = new Object(key); 
	p->setObjList(); 
	objstack.push_back(p); 
}


void setRHSleaf(char const* first, char const* last)
{
	string val(first, last); 
	Object* l = stack.back();
	stack.pop_back(); 
	l->setValue(val); 
	if (0 < stack.size())
	{
		Object* p = stack.back(); 
		p->setValue(l); 
	}
}

void setRHStaglist(char const* first, char const* last)
{
	Object* l = stack.back();
	stack.pop_back(); 

	string val(first, last); 
	string tag;
	bool stringmode = false; 
	for (unsigned int i = (val[0] == '{') ? 1 : 0; i < val.size(); ++i)
	{
		if ((val[i] == ' ') && (0 == tag.size()))
		{
			continue;
		}
		if (val[i] == '"')
		{
			tag.push_back(val[i]); 
			if (stringmode)
			{
				l->addToList(tag); 
				tag.clear(); 
			}
			stringmode = !stringmode;
			continue;
		}
		if (((val[i] == ' ') && (!stringmode)) || (val[i] == '}'))
		{
	      if (tag.size() > 0)
			{
				l->addToList(tag); 
			}
			tag.clear(); 
			continue;
		}
		tag.push_back(val[i]); 
	}

	if (0 < stack.size())
	{
		Object* p = stack.back(); 
		p->setValue(l); 
	}
}

void setRHSobject(char const* first, char const* last)
{
	// No value is set, a bunch of leaves have been defined. 
	Object* l = stack.back();
	stack.pop_back(); 
	if (0 < stack.size())
	{
		Object* p = stack.back(); 
		p->setValue(l); 
	}
}

void setRHSobjlist(char const* first, char const* last)
{
	if (0 < stack.size())
	{
		Object* p = stack.back(); 
		p->setValue(objstack); 
	}
	stack.pop_back(); 
	objstack.clear(); 
}


bool makeObject(string& command)
{
	//cout << "Entering makeObject with " << command << endl;
	static Parser p;
	//BOOST_SPIRIT_DEBUG_GRAMMAR(p);
	parse_info<> result = parse(command.c_str(), p, nothing_p);
	if (result.full)
	{
		//cout << "Matched " << command << endl;
		// Do stuff with full command

		//command.clear();
		return true;
	}

	//cout << "No match in " << command << " stopped at \"" << result.stop << "\"" << endl;
	return false;
}




