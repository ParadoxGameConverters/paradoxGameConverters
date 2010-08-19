#ifndef DATEPARSER_HH
#define DATEPARSER_HH

//#define BOOST_SPIRIT_DEBUG 1
//#define BOOST_SPIRIT_DEBUG_FLAGS BOOST_SPIRIT_DEBUG_FLAGS_NODES

#include "boost/spirit.hpp"
#include <string> 
#include <iostream> 
#include <fstream> 


using namespace boost::spirit;

void setYear (unsigned int y);
void setMonth (unsigned int m);
void setDay (unsigned int d); 

struct DateParser : public boost::spirit::grammar<DateParser> {
  static int currYear;
  static int currMonth;
  static int currDay; 

  static const unsigned int DATE  = 103;

  template<typename ScannerT> struct definition {
    boost::spirit::rule<ScannerT, parser_context<>, parser_tag<DATE> > date;

    definition (DateParser const& self) {
      date = ( uint_p[&setYear] >> ch_p('.') >> uint_p[&setMonth] >> ch_p('.') >> uint_p[&setDay] );
      BOOST_SPIRIT_DEBUG_RULE(date);
    }

    boost::spirit::rule<ScannerT, parser_context<>, parser_tag<DATE> > const& start() const { return date; }
  };

private:

};


#endif
