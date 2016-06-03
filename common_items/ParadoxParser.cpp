/*Copyright (c) 2016 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



/*Copyright (c) 2010 Rolf Andreassen
 
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#pragma warning(disable : 4348)	// suppress warnings from Spirit, because they aren't being fixed (or the fixes aren't being released)
#include "ParadoxParser.h"
#include <fstream>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/spirit/include/qi.hpp>
#include "Log.h"



using namespace boost::spirit;



namespace parser_8859_15
{



// if an automated way can be found to do this, use that instead. But be wary of UTF-8 converters, they choke on 8859-15
wstring convert8859_15ToWideString(string input)
{
	wstring output;
	for (auto character: input)
	{
		if (character > 0)
		{
			output.push_back(character);
		}
		else
		{
			switch (static_cast<unsigned char>(character))
			{
				case 160:
					output.push_back(L' ');
					break;
				case 161:
					output.push_back(L'¡');
					break;
				case 162:
					output.push_back(L'¢');
					break;
				case 163:
					output.push_back(L'£');
					break;
				case 164:
					output.push_back(L'€');
					break;
				case 165:
					output.push_back(L'¥');
					break;
				case 166:
					output.push_back(L'Š');
					break;
				case 167:
					output.push_back(L'§');
					break;
				case 168:
					output.push_back(L'š');
					break;
				case 169:
					output.push_back(L'©');
					break;
				case 170:
					output.push_back(L'ª');
					break;
				case 171:
					output.push_back(L'«');
					break;
				case 172:
					output.push_back(L'¬');
					break;
				case 173:
					output.push_back(L'-');
					break;
				case 174:
					output.push_back(L'®');
					break;
				case 175:
					output.push_back(L'¯');
					break;
				case 176:
					output.push_back(L'°');
					break;
				case 177:
					output.push_back(L'±');
					break;
				case 178:
					output.push_back(L'²');
					break;
				case 179:
					output.push_back(L'³');
					break;
				case 180:
					output.push_back(L'Ž');
					break;
				case 181:
					output.push_back(L'µ');
					break;
				case 182:
					output.push_back(L'¶');
					break;
				case 183:
					output.push_back(L'·');
					break;
				case 184:
					output.push_back(L'ž');
					break;
				case 185:
					output.push_back(L'¹');
					break;
				case 186:
					output.push_back(L'º');
					break;
				case 187:
					output.push_back(L'»');
					break;
				case 188:
					output.push_back(L'Œ');
					break;
				case 189:
					output.push_back(L'œ');
					break;
				case 190:
					output.push_back(L'Ÿ');
					break;
				case 191:
					output.push_back(L'¿');
					break;
				case 192:
					output.push_back(L'À');
					break;
				case 193:
					output.push_back(L'Á');
					break;
				case 194:
					output.push_back(L'Â');
					break;
				case 195:
					output.push_back(L'Ã');
					break;
				case 196:
					output.push_back(L'Ä');
					break;
				case 197:
					output.push_back(L'Å');
					break;
				case 198:
					output.push_back(L'Æ');
					break;
				case 199:
					output.push_back(L'Ç');
					break;
				case 200:
					output.push_back(L'È');
					break;
				case 201:
					output.push_back(L'É');
					break;
				case 202:
					output.push_back(L'Ê');
					break;
				case 203:
					output.push_back(L'Ë');
					break;
				case 204:
					output.push_back(L'Ì');
					break;
				case 205:
					output.push_back(L'Í');
					break;
				case 206:
					output.push_back(L'Î');
					break;
				case 207:
					output.push_back(L'Ï');
					break;
				case 208:
					output.push_back(L'Ð');
					break;
				case 209:
					output.push_back(L'Ñ');
					break;
				case 210:
					output.push_back(L'Ò');
					break;
				case 211:
					output.push_back(L'Ó');
					break;
				case 212:
					output.push_back(L'Ô');
					break;
				case 213:
					output.push_back(L'Õ');
					break;
				case 214:
					output.push_back(L'Ö');
					break;
				case 215:
					output.push_back(L'×');
					break;
				case 216:
					output.push_back(L'Ø');
					break;
				case 217:
					output.push_back(L'Ù');
					break;
				case 218:
					output.push_back(L'Ú');
					break;
				case 219:
					output.push_back(L'Û');
					break;
				case 220:
					output.push_back(L'Ü');
					break;
				case 221:
					output.push_back(L'Ý');
					break;
				case 222:
					output.push_back(L'Þ');
					break;
				case 223:
					output.push_back(L'ß');
					break;
				case 224:
					output.push_back(L'à');
					break;
				case 225:
					output.push_back(L'á');
					break;
				case 226:
					output.push_back(L'â');
					break;
				case 227:
					output.push_back(L'ã');
					break;
				case 228:
					output.push_back(L'ä');
					break;
				case 229:
					output.push_back(L'å');
					break;
				case 230:
					output.push_back(L'æ');
					break;
				case 231:
					output.push_back(L'ç');
					break;
				case 232:
					output.push_back(L'è');
					break;
				case 233:
					output.push_back(L'é');
					break;
				case 234:
					output.push_back(L'ê');
					break;
				case 235:
					output.push_back(L'ë');
					break;
				case 236:
					output.push_back(L'ì');
					break;
				case 237:
					output.push_back(L'í');
					break;
				case 238:
					output.push_back(L'î');
					break;
				case 239:
					output.push_back(L'ï');
					break;
				case 240:
					output.push_back(L'ð');
					break;
				case 241:
					output.push_back(L'ñ');
					break;
				case 242:
					output.push_back(L'ò');
					break;
				case 243:
					output.push_back(L'ó');
					break;
				case 244:
					output.push_back(L'ô');
					break;
				case 245:
					output.push_back(L'õ');
					break;
				case 246:
					output.push_back(L'ö');
					break;
				case 247:
					output.push_back(L'÷');
					break;
				case 248:
					output.push_back(L'ø');
					break;
				case 249:
					output.push_back(L'ù');
					break;
				case 250:
					output.push_back(L'ú');
					break;
				case 251:
					output.push_back(L'û');
					break;
				case 252:
					output.push_back(L'ü');
					break;
				case 253:
					output.push_back(L'ý');
					break;
				case 254:
					output.push_back(L'þ');
					break;
				case 255:
					output.push_back(L'ÿ');
					break;
				default:
					;// do nothing
			}
		}
	}
	return output;
}


static void setLHS						(string key);
static void pushObj						();
static void setRHSleaf					(string val);
static void setRHSobject				();
static void setRHSobjlist				();
static void endRHSobjlist				();
static void setRHStaglist				(vector<string> val);
static void setEpsilon					();
static void setAssign					();

static Object*		topLevel		= NULL;  // a top level object
vector<Object*>	stack;					// a stack of objects
vector<Object*>	objstack;				// a stack of objects
bool					epsilon		= false;	// if we've tried an episilon for an assign
bool					inObjList	= false;	// if we're inside an object list


template <typename Iterator>
struct SkipComment : qi::grammar<Iterator>
{
	qi::rule<Iterator> comment;	// the rule for identiying comments

	SkipComment() : SkipComment::base_type(comment)
	{
		comment = qi::raw[qi::lexeme[lit("#") >> *(iso8859_1::char_ - qi::eol)] >> -qi::eol];
	}
};


static bool debugme = false;	// whether or not debugging should be on
template <typename Iterator>
struct Parser : public qi::grammar<Iterator, SkipComment<Iterator> > {
	static Object* topLevel; 

	// leaf: either left or right side of assignment.  unquoted keyword.
	// example: leaf
	qi::rule<Iterator, string(), SkipComment<Iterator> >	leaf;

	// taglist: a grouping of anonymous (rhs) leaves or strings
	// examples: { TAG TAG TAG } or { "string" "string" TAG }
	qi::rule<Iterator, vector<string>(), SkipComment<Iterator> >	taglist;

	// assign: assignment
	// examples: lhs = rhs or lhs = { lhs = rhs }
	qi::rule<Iterator, SkipComment<Iterator> >	assign;

	// object: a grouping of assignments
	// example: object = { leaf = "string" }
	qi::rule<Iterator, SkipComment<Iterator> >	object;

	// objlist: a grouping of anonymous (rhs) objects
	// example: objlist = { { leaf = "string" } { leaf = leaf } }
	qi::rule<Iterator, SkipComment<Iterator> >	objlist;

	// str: a quoted literal string.  may include extended and/or reserved characters.
	// example: "I am a string."
	qi::rule<Iterator, string()>	str;

	// tolleaf: a tolerant leaf.  may include extended and other unreserved characters.  rhs only.
	// example: leaves with accents (names, for instance).
	qi::rule<Iterator, string(), SkipComment<Iterator> >	tolleaf;

	// braces: a stray set of empty rhs braces (without an lhs)
	// EU3 seems to do this for certain decision mods.
	qi::rule<Iterator, SkipComment<Iterator> >	braces;

	// root: evaluation starts here.  objects recurse to root.
	qi::rule<Iterator, SkipComment<Iterator> >	root;

	Parser() : Parser::base_type(root)
	{
		// { }
		braces = *(iso8859_1::space) >> lit('{') >> *(iso8859_1::space) >> lit('}') >> *(iso8859_1::space);

		// a string enclosed in quotes
		str     = lexeme[lit('"') >> raw[*(~iso8859_1::char_('"') | lit(0x80) | lit(0x81) | lit(0x82) | lit(0x83) | lit(0x84) | lit(0x85) | lit(0x86) | lit(0x87) | lit(0x88) | lit(0x89) | lit(0x8A) | lit(0x8B) | lit(0x8C) | lit(0x8D) | lit(0x8E) | lit(0x8F) | lit(0x90) | lit(0x91) | lit(0x92) | lit(0x93) | lit(0x94) | lit(0x95) | lit(0x96) | lit(0x97) | lit(0x98) | lit(0x99) | lit(0x9A) | lit(0x9B) | lit(0x9C) | lit(0x9D) | lit(0x9E) | lit(0x9F) | lit('–') | lit('&'))] >> lit('"')];

		// a 'forgiving' string without quotes
		tolleaf = raw[+(~iso8859_1::char_("\"{}= \t\r\n") | lit(0x80) | lit(0x81) | lit(0x82) | lit(0x83) | lit(0x84) | lit(0x85) | lit(0x86) | lit(0x87) | lit(0x88) | lit(0x89) | lit(0x8A) | lit(0x8B) | lit(0x8C) | lit(0x8D) | lit(0x8E) | lit(0x8F) | lit(0x90) | lit(0x91) | lit(0x92) | lit(0x93) | lit(0x94) | lit(0x95) | lit(0x96) | lit(0x97) | lit(0x98) | lit(0x99) | lit(0x9A) | lit(0x9B) | lit(0x9C) | lit(0x9D) | lit(0x9E) | lit(0x9F) | lit('–') | lit('&'))];

		// a strict string without quotes
		leaf    = raw[+(iso8859_1::alnum | iso8859_1::char_("-._:") | lit(0x80) | lit(0x81) | lit(0x82) | lit(0x83) | lit(0x84) | lit(0x85) | lit(0x86) | lit(0x87) | lit(0x88) | lit(0x89) | lit(0x8A) | lit(0x8B) | lit(0x8C) | lit(0x8D) | lit(0x8E) | lit(0x8F) | lit(0x90) | lit(0x91) | lit(0x92) | lit(0x93) | lit(0x94) | lit(0x95) | lit(0x96) | lit(0x97) | lit(0x98) | lit(0x99) | lit(0x9A) | lit(0x9B) | lit(0x9C) | lit(0x9D) | lit(0x9E) | lit(0x9F) | lit('–') | lit('&'))];

		// a list of strings within brackets
		taglist = lit('{') >> omit[*(iso8859_1::space)] >> lexeme[( ( str | skip[tolleaf] ) % *(iso8859_1::space) )] >> omit[*(iso8859_1::space)] >> lit('}');

		// a root object contained within brackets
		object  = raw[lit('{') >> *(root) >> *(iso8859_1::space) >> lit('}')];

		// a list of objects contained within brackets
		objlist = raw[lit('{') >> *( *(iso8859_1::space) >> object[&pushObj] ) >> *(iso8859_1::space) >> lit('}')];

		// an assignment. Left side is a string of some kind, right side is one of many allowed types
		assign  = raw[(*(iso8859_1::space) >> ( leaf[&setLHS] | str[&setLHS] | eps[&setEpsilon] ) >> *(iso8859_1::space)
			>>	lit('=')[&setAssign]	>> *(iso8859_1::space) 
			>> ( leaf[&setRHSleaf] | str[&setRHSleaf] | taglist[&setRHStaglist] | objlist[&setRHSobjlist] | object[&setRHSobject] ) >> *(iso8859_1::space))];

		// the root object (either an assignment, or something in braces)
		root	= +(assign | braces);

		str.name("str");
		leaf.name("leaf");
		taglist.name("taglist");
		object.name("object");
		objlist.name("objlist");
		assign.name("assign");
		braces.name("braces");
		root.name("root");

		if (debugme)
		{
			debug(str);
			debug(leaf);
			debug(taglist);
			debug(object);
			debug(objlist);
			debug(assign);
			debug(braces);
			debug(root);
		}
	}
};


Object* getTopLevel()
{
	return topLevel;
}


void initParser()
{
	topLevel = new Object(L"topLevel");
	epsilon	= false;
}


string bufferOneObject(ifstream& read)
{
	int openBraces = 0;				// the number of braces deep we are
	string currObject, buffer;		// the current object and the tect under consideration
	bool topLevel = true;			// whether or not we're at the top level
	while (read.good())
	{
		getline(read, buffer);
		if (buffer == "CK2txt")
		{
			continue;
		}
		else if (buffer == "EU4txt")
		{
			continue;
		}
		currObject += "\n";

		bool opened = false;									// whether or not we just opened a new brace level
		bool isInLiteral = false;							// whether or not we're in a string literal
		const char* str = buffer.c_str();				// a character string of the text under consideration
		const unsigned int strSize = buffer.size();	// the size of the text under consideration
		for (unsigned int i = 0; i < strSize; ++i)
		{
			if ('"' == str[i])
			{
				isInLiteral = !isInLiteral;
			}
			if (isInLiteral)
			{
				currObject += str[i];
				continue;
			}
			if ('#' == str[i])
			{
				break;
			}
			if ('{' == str[i])
			{
				++openBraces;
				opened = true;
			}
			else if ('}' == str[i])
			{
				--openBraces;
			}
			currObject += str[i];
		}

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

		break;
	}
	return currObject;
}


bool readFile(ifstream& read)
{
	clearStack();
	read.unsetf(std::ios::skipws);

	char firstChar = read.peek();
	if (firstChar == (char)0xEF)
	{
		char bitBucket[3];
		read.read(bitBucket, 3);
		LOG(LogLevel::Warning) << "Identified a BOM in a file that should be UTF-8";
	}

	/* - it turns out that the current implementation of spirit::istream_iterator is ungodly slow...
	static Parser<boost::spirit::istream_iterator> p;
	static SkipComment<boost::spirit::istream_iterator> s;

	boost::spirit::istream_iterator begin(read);
	boost::spirit::istream_iterator end;

	return qi::phrase_parse(begin, end, p, s);
	*/

	const static Parser<string::iterator> p;
	const static SkipComment<string::iterator> s;

	/* buffer and parse one object at a time */
	while (read.good())
	{
		string currObject = bufferOneObject(read);	// the object under consideration

		if (!qi::phrase_parse(currObject.begin(), currObject.end(), p, s))
		{
			clearStack();
			return false;
		}
	}

	clearStack();
	return true;
}


void clearStack()
{
	if (!stack.empty())
	{
		Log logOutput(LogLevel::Warning);	// a section in the log file that won't automatically be broken into lines
		logOutput << "Clearing stack size " << stack.size() << " - this should not happen in normal operation\n";
		for (vector<Object*>::iterator i = stack.begin(); i != stack.end(); ++i)
		{
			logOutput << **i << '\n';
		}
	}
	stack.clear();
}


void setLHS(string key)
{
	//LOG(LogLevel::Debug) << "Setting LHS : " << key;

	wstring wide_key = convert8859_15ToWideString(key);
	Object* p = new Object(wide_key);
	if (0 == stack.size())
	{
		topLevel->setValue(p);
	}
	stack.push_back(p);
	epsilon = false;
}


void pushObj()
{
	inObjList = true;
	//LOG(LogLevel::Debug) << "Pushing objlist";
	wstring key(L"objlist");			// the key of the object list
	Object* p = new Object(key);	// the object to hold the object list
	p->setObjList(); 
	objstack.push_back(p); 
}


void setRHSleaf(string val)
{
	wstring wide_val = convert8859_15ToWideString(val);

	//LOG(LogLevel::Debug) << "Setting RHSleaf : " << val;
	Object* l = stack.back();	// the leaf object
	stack.pop_back(); 
	l->setValue(wide_val);
	if ( (!inObjList) &&(0 < stack.size()) )
	{
		Object* p = stack.back();	// the object holding the leaf
		p->setValue(l); 
	}
	else if ( (inObjList) &&(0 < objstack.size()) )
	{
		Object* p = objstack.back();	// the object holding the leaf
		p->setValue(l); 
	}
}


void setRHStaglist(vector<string> val)
{
	vector<wstring> wide_vals;
	for (auto aVal: val)
	{
		wstring wide_val = convert8859_15ToWideString(aVal);
		wide_vals.push_back(wide_val);
	}

	//LOG(LogLevel::Debug) << "Setting RHStaglist";
	Object* l = stack.back();	// the object holding the list
	stack.pop_back(); 
	l->addToList(wide_vals.begin(), wide_vals.end());
	if ( (!inObjList) &&(0 < stack.size()) )
	{
		Object* p = stack.back();	// the object holding the leaf
		p->setValue(l); 
	}
	else if ( (inObjList) &&(0 < objstack.size()) )
	{
		Object* p = objstack.back();	// the object holding the leaf
		p->setValue(l); 
	}
}


void setRHSobject()
{
	//LOG(LogLevel::Debug) << "Setting RHSobject";
	// No value is set, a bunch of leaves have been defined. 
	Object* l = stack.back();
	stack.pop_back(); 
	if ( (!inObjList) &&(0 < stack.size()) )
	{
		Object* p = stack.back();	// the object holding the leaf
		p->setValue(l); 
	}
	else if ( (inObjList) &&(0 < objstack.size()) )
	{
		Object* p = objstack.back();	// the object holding the leaf
		p->setValue(l); 
	}
}


void setRHSobjlist()
{
	inObjList = false;
	//LOG(LogLevel::Debug) << "Setting RHSobjlist";
	Object* l = stack.back();	// the object
	l->setValue(objstack);
	objstack.clear();
	stack.pop_back(); 
	if (0 < stack.size())
	{
		Object* p = stack.back(); // the other object
		p->setValue(l); 
	}
}


void setEpsilon()
{
	//LOG(LogLevel::Debug) << "Setting epsilon";
	epsilon = true;
}


void setAssign()
{
	//LOG(LogLevel::Debug) << "In assign";
	if (epsilon)
	{
		Object* e = new Object(L"epsilon");
		stack.push_back(e);
	}
	epsilon = false;
}


Object* doParseFile(const wchar_t* filename)
{
	ifstream	read;				// ifstream for reading files

	/* - when using parser debugging, also ensure that the parser object is non-static!
	debugme = false;
	if (string(filename) == "D:\\Victoria 2\\technologies\\commerce_tech.txt")
		debugme = true;
	*/

	initParser();
	Object* obj = getTopLevel();	// the top level object
	read.open(filename); 
	if (!read.is_open())
	{
		return NULL;
	}
	readFile(read);  
	read.close();
	read.clear();

	return obj;
}



} // namespace parser_8859_15