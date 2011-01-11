#include "Parser.h"
#include "Object.h"

Object* Parser::topLevel = NULL;  
std::vector<Object*> stack; 
std::vector<Object*> objstack; 

void initParser()
{
   Parser::topLevel = new Object("topLevel");
}

int trim (std::string& str) {
  // Returns net number of braces opened by str, and trims leading and trailing whitespace.

  const char* s = str.c_str();
  int ret = 0; 
  unsigned int strSize = str.size();
  if (0 == strSize) return 0;
  bool isInLiteral = false; 
  for (unsigned int i = 0; i < strSize; ++i) {
    if ('"' == s[i]) isInLiteral = !isInLiteral; 
    if (isInLiteral) continue; 
    if ('{' == s[i]) ++ret; 
    else if ('}' == s[i]) --ret;
  }

  unsigned int first = 0;
  for (; first < strSize; ++first) {
    if (s[first] == ' ') continue;
    if (13 == (int) s[first]) continue; // Carriage return
    break;
  }

  unsigned int last = strSize - 1; 
  for (; last > first; --last) {
    if (' ' == s[last]) continue;
    if (13 == (int) s[last]) continue; 
    break;
  }

  unsigned int fcomment = 0;
  for (; fcomment < strSize; ++fcomment) {
    if (s[fcomment] != '#') continue; 
    break;
  }
  if (fcomment < last) last = fcomment-1; 

  str = str.substr(first, last - first + 1); 
  return ret; 
}

void readFile (std::ifstream& read) {
  clearStack(); 
  int openBraces = 0;
  std::string currObject;  
  bool topLevel = true; 

  while (read.good()) {
    std::string buffer;
    std::getline(read, buffer);

    int currBraces = trim(buffer); 
    openBraces += currBraces; 
    currObject += " " + buffer; 

    if (openBraces > 0) {
      topLevel = false;
      continue; 
    }
    if (currObject == "") continue; 

    // Don't try to end an object that hasn't started properly;
    // accounts for such constructions as
    // object = 
    // { 
    // where openBraces is zero after the first line of the object. 
    // Not a problem for non-top-level objects since these will have
    // nonzero openBraces anyway. 
    if (topLevel) continue; 

    openBraces = 0; 
    makeObject(currObject);
    currObject.clear(); 
    topLevel = true; 
  }

  trim(currObject); 
  // MAX: Ignore this 
  /*
  if (currObject.size() > 0) {
    std::cout << "Warning: Unable to parse file; problem is with \"" << currObject << "\"" << std::endl; 
    std::cout << "Aborting, unable to parse file" << std::endl; 
    abort();
  }
  */
}


void clearStack () {
  if (0 < stack.size()) std::cout << "Warning: Clearing stack size " 
				  << stack.size() 
				  << ". This should not happen in normal operation." 
				  << std::endl; 
  for (std::vector<Object*>::iterator i = stack.begin(); i != stack.end(); ++i) {
    //std::cout << (*i)->getKey() << std::endl; 
    std::cout << (*(*i)) << std::endl; 
  }
  stack.clear();
}

void setLHS (char const* first, char const* last) {
  std::string key(first, last); 
  //std::cout << "Setting lefthand side " << key << std::endl; 
  Object* p = new Object(key); 
  if (0 == stack.size()) {
    Parser::topLevel->setValue(p);
  }
  stack.push_back(p); 
}

void pushObj (char const* first, char const* last) {
  std::string key("objlist");
  Object* p = new Object(key); 
  p->setObjList(); 
  objstack.push_back(p); 
}


void setRHSleaf (char const* first, char const* last) {
  std::string val(first, last); 
  Object* l = stack.back();
  stack.pop_back(); 
  l->setValue(val); 
  if (0 < stack.size()) {
    Object* p = stack.back(); 
    p->setValue(l); 
  }
}

void setRHStaglist (char const* first, char const* last) {
  Object* l = stack.back();
  stack.pop_back(); 

  std::string val(first, last); 
  std::string tag;
  bool stringmode = false; 
  for (unsigned int i = (val[0] == '{') ? 1 : 0; i < val.size(); ++i) {
    if ((val[i] == ' ') && (0 == tag.size())) continue; 
    if (val[i] == '"') {
      tag.push_back(val[i]); 
      if (stringmode) {
	l->addToList(tag); 
	tag.clear(); 
      }
      stringmode = !stringmode;
      continue;
    }
    if (((val[i] == ' ') && (!stringmode)) || (val[i] == '}')) {
      if (tag.size() > 0) l->addToList(tag); 
      tag.clear(); 
      continue;
    }
    tag.push_back(val[i]); 
  }

  if (0 < stack.size()) {
    Object* p = stack.back(); 
    p->setValue(l); 
  }
}

void setRHSobject (char const* first, char const* last) {
  // No value is set, a bunch of leaves have been defined. 
  Object* l = stack.back();
  stack.pop_back(); 
  if (0 < stack.size()) {
    Object* p = stack.back(); 
    p->setValue(l); 
  }
}

void setRHSobjlist (char const* first, char const* last) {
  if (0 < stack.size()) {
    Object* p = stack.back(); 
    p->setValue(objstack); 
  }
  stack.pop_back(); 
  objstack.clear(); 
}


bool makeObject (std::string& command) {
  //std::cout << "Entering makeObject with " << command << std::endl; 
  static Parser p;
  //BOOST_SPIRIT_DEBUG_GRAMMAR(p);
  boost::spirit::parse_info<> result = boost::spirit::parse(command.c_str(), p, boost::spirit::nothing_p);
  if (result.full) {
    //std::cout << "Matched " << command << std::endl; 
    // Do stuff with full command

    //command.clear(); 
    return true; 
  }

  //std::cout << "No match in " << command << " stopped at \"" << result.stop << "\"" << std::endl;
  return false; 
}




