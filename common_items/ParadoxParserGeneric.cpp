#include "ParadoxParserGeneric.h"

#include "ConversionFacet.h"

#include <fstream>
#include <locale>

#include <boost/bind.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

using namespace parser_generic;

Object *parser_generic::parseUTF_8(const std::string &file_path){
	return parse(file_path, UTF_8);
}

Object *parser_generic_parseISO_8859_15(const std::string &file_path){
	return parse(file_path, ISO_8859_15);
}

Object *parser_generic::parse(const std::string &file_path, Encoding file_encoding){
	using namespace std;
	
	//open the file as a wide character stream
	basic_ifstream<wchar_t> input{file_path};

	//create a new local based on the old one, but using the codecvt facet to decode the file encoding to the system dependant WCHAR encoding
	locale loc{input.getloc(), new ConversionFacet<wchar_t,char>{file_encoding, WCHAR}};
	
	/*
	input will now interpret the file as encoded by the specified encoding and transform the output into wide characters using the system dependentencoding
	this will be UCS (4 byte codepoints) in most linux distro's and UTF-16 LE on windows
	because the system encoding is not specifically named, the parser is portable if the ConversionFacet codecvt facet is implemented on all target systems
	*/
	input.imbue(loc);

	//delegate to parse, which won't need to care about the encoding anymore 
	return parse(input);	
}

// forward declaration of the parse function to keep things clean
template<typename Iterator> Object *do_parse(Iterator begin, Iterator end);

Object *parser_generic::parse(std::basic_istream<wchar_t> &input){
	
	/*
	use boost's istream_iterator to directly couple the istream to the parser
	this makes sure we can parse the entire file without preparsing it into objects and buffering each one (bufferNextObject will no longer be needed)
	note that the entire file is not loaded into memory, and all buffering is done in istream's buffers
	*/
	boost::spirit::basic_istream_iterator<wchar_t> begin(input);
        boost::spirit::basic_istream_iterator<wchar_t> end;		
	
	do_parse(begin, end);
}

/*
	TODO: implement the parser properly by porting the grammar from ISO8859_15 parser
	The current parser is a dummy that doesn't really do anything yet but demonstrate the eventual code
	Note that the parser uses the system dependent wchar_t encoding but is still portable because the actual decoding of the files is done elsewhere
*/

namespace qi = boost::spirit::qi;

namespace parsers = qi::standard_wide;

const wchar_t QUOTE = L'"';

const wchar_t ASSIGNMENT = L'=';

const wchar_t OBJECT_START = L'{';

const wchar_t OBJECT_END = L'}';

const wchar_t COMMENT = L'#';

//DUMMY
void print(const std::wstring &str){
        using namespace std;
        cout << "quoted string: \"";
        for(wchar_t c : str){
                cout << static_cast<int>(c) << " ";
        }
        cout << '\"'<< endl;
};

//DUMMY
template<typename Iterator> class Grammar : public qi::grammar<Iterator, std::wstring()>{
private:

        qi::rule<Iterator, std::wstring()> quoted_string_;

public:
        Grammar() : Grammar::base_type(quoted_string_){
                using parsers::char_;
                using parsers::space;
                using parsers::graph;

                quoted_string_ = (QUOTE >> *(char_ - QUOTE) >> QUOTE);

        };

};

//DUMMY
template<typename Iterator> Object *do_parse(Iterator begin, Iterator end){
        using namespace std;

        using qi::rule;
        using qi::parse;

        using parsers::char_;
        using parsers::space;
        using parsers::graph;

        Grammar<Iterator> grammar;

        wstring str;

        bool result = true;
        while(result){
                result = parse(begin, end, grammar, str);
                cout << "result: " << result << endl;
                if(result){
                        print(str);
                }
        }
	return nullptr;
};
