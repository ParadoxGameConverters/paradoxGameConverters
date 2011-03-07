#ifndef PARSER_HH
#define PARSER_HH

//#define BOOST_SPIRIT_DEBUG 1
//#define BOOST_SPIRIT_DEBUG_FLAGS BOOST_SPIRIT_DEBUG_FLAGS_NODES

#undef BOOST_SPIRIT_DEBUG 
#undef BOOST_SPIRIT_DEBUG_FLAGS 

#include "boost/spirit/include/classic.hpp"
#include "boost/spirit/include/classic_ast.hpp"
#include <string> 
#include "Object.h"
#include <iostream> 
#include <fstream> 


using namespace boost::spirit::classic;
void setLHS (char const* first, char const* last);
void pushObj (char const* first, char const* last);
void setRHSleaf (char const* first, char const* last);
void setRHSobject (char const* first, char const* last);
void setRHSobjlist (char const* first, char const* last);
void setRHStaglist (char const* first, char const* last);

struct Parser : public boost::spirit::classic::grammar<Parser> {
  static Object* topLevel; 

  static const unsigned int OBJECT  = 1;
  static const unsigned int LEAF    = 2;
  static const unsigned int TAGLIST = 3;
  static const unsigned int ASSIGN  = 4;
  static const unsigned int STR     = 5;
  static const unsigned int OBJLIST = 6;

  template<typename ScannerT> struct definition {
    boost::spirit::classic::rule<ScannerT, parser_context<>, parser_tag<LEAF> >     leaf;
    boost::spirit::classic::rule<ScannerT, parser_context<>, parser_tag<TAGLIST> >  taglist;
    boost::spirit::classic::rule<ScannerT, parser_context<>, parser_tag<ASSIGN> >   assign; 
    boost::spirit::classic::rule<ScannerT, parser_context<>, parser_tag<OBJECT> >   object;
    boost::spirit::classic::rule<ScannerT, parser_context<>, parser_tag<OBJLIST> >  objlist;
    boost::spirit::classic::rule<ScannerT, parser_context<>, parser_tag<STR> >      str;

    definition (Parser const& self) {
      str     = ch_p('"') >> *(anychar_p - ch_p('"') ) >> ch_p('"');
      leaf    = ( eps_p( ((anychar_p - ch_p('=')) - ch_p('{') ) - ch_p('}') - ch_p('"') ) >> *(graph_p - ch_p('=') - ch_p('}')));
      taglist = ch_p('{') >> *( *(blank_p) >> ( leaf | str ) ) >> *(blank_p) >> ch_p('}');
      object  = ch_p('{') >> +(assign) >> *(blank_p) >> ch_p('}');
      objlist = ch_p('{') >> *( *(blank_p) >> object[&pushObj] ) >> *(blank_p) >> ch_p('}');
      assign  = +((*(blank_p) >> (leaf[&setLHS] | str[&setLHS]) >> *(blank_p) >> ch_p('=') 
		   >> *(blank_p) 
		   >> ( leaf[&setRHSleaf] | str[&setRHSleaf] | taglist[&setRHStaglist] | objlist[&setRHSobjlist] | object[&setRHSobject] ) 
		   >> *(blank_p)));

      /*BOOST_SPIRIT_DEBUG_RULE(leaf);
      BOOST_SPIRIT_DEBUG_RULE(taglist);
      BOOST_SPIRIT_DEBUG_RULE(assign);
      BOOST_SPIRIT_DEBUG_RULE(object);
      BOOST_SPIRIT_DEBUG_RULE(objlist);
      BOOST_SPIRIT_DEBUG_RULE(str);*/

    }

    boost::spirit::classic::rule<ScannerT, parser_context<>, parser_tag<ASSIGN> > const& start() const { return assign; }
  };

private:

};

void clearStack (); 
bool makeObject (std::string& command);
void readFile (std::ifstream& read);
int trim (std::string& str); 
void initParser();

#endif
