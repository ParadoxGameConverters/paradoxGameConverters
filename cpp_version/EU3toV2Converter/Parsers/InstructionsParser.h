#ifndef INSTR_PARSER_HH
#define INSTR_PARSER_HH

//#define BOOST_SPIRIT_DEBUG 1
//#define BOOST_SPIRIT_DEBUG_FLAGS BOOST_SPIRIT_DEBUG_FLAGS_NODES

#undef BOOST_SPIRIT_DEBUG 
#undef BOOST_SPIRIT_DEBUG_FLAGS 

#include "..\stdafx.h"
#include "boost/spirit/include/classic.hpp"
#include "boost/spirit/include/classic_ast.hpp"
#include <string> 
#include <iostream> 
#include <fstream> 

using namespace boost::spirit::classic;
void setVarType (char const* first, char const* last);
void setVarName (char const* first, char const* last);

void newVarOrRule (char const* first, char const* last);
void getVarRuleName (char const* first, char const* last);
void parseInstruction(char const* first, char const* last);
void parseRuleParam(char const* first, char const* last);

void readInsFile (std::ifstream& read);

struct InstructionsParser : public boost::spirit::classic::grammar<InstructionsParser> {

  static std::vector<varDefinition> GetProcessedVars();
  static std::vector<ruleset> GetProcessedRulesets();
  static void Refresh();

  static const unsigned int PROCESS = 1;
  static const unsigned int STR     = 2;  
  static const unsigned int VARDEF  = 3;  
  static const unsigned int RULEDEF  = 4;  
  static const unsigned int INSTR     = 5;

  template<typename ScannerT> struct definition {
    boost::spirit::classic::rule<ScannerT, parser_context<>, parser_tag<PROCESS> >  process;
    boost::spirit::classic::rule<ScannerT, parser_context<>, parser_tag<STR> >      str;    
    boost::spirit::classic::rule<ScannerT, parser_context<>, parser_tag<VARDEF> >   varDef;
    boost::spirit::classic::rule<ScannerT, parser_context<>, parser_tag<RULEDEF> >  ruleDef;
    boost::spirit::classic::rule<ScannerT, parser_context<>, parser_tag<INSTR> >	   instr;

    definition (InstructionsParser const& self) {
      str    = *(anychar_p - ch_p(' ') - ch_p('\t') - ch_p('\n') - ch_p('[') - ch_p(']') );
      instr  = *(anychar_p - ch_p('\n') - ch_p('[') - ch_p(']'));
      varDef = ch_p('[') >> *(blank_p) >> str[&newVarOrRule] >> *(blank_p) >> str[&getVarRuleName] >> *(blank_p) >> ch_p(']') >> *(blank_p) >> ch_p('\n');      
      ruleDef = ch_p('[') >> ch_p('[') >> *(blank_p) >> str[&newVarOrRule] >> *(blank_p) >> str[&getVarRuleName] >> *(blank_p) >> ch_p(']') >> ch_p(']') >> *(blank_p) >> ch_p('\n');
      // process = *( varDef >> *(instr[&parseInstruction] >> ch_p('\n')) );
      process = *( varDef >> *( instr[&parseInstruction] >> ch_p('\n') ) ) >> *( ruleDef >> *( instr[&parseRuleParam] >> ch_p('\n') ) );

      BOOST_SPIRIT_DEBUG_RULE(process);
    }

    boost::spirit::classic::rule<ScannerT, parser_context<>, parser_tag<PROCESS> > const& start() const { return process; }
  };

private:

};

#endif
