#include "ParadoxParserGeneric.h"
#include "OSCompatibilityLayer.h"
#include "Log.h"

#include "ConversionFacet.h"

#include <fstream>
#include <locale>
#include <stack>
#include <string>
#include <boost/bind.hpp>

#include <boost/bind.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

using namespace parser_generic;

Object *parser_generic::parseUTF_8(const std::string &file_path){
	return parser_generic::parse(file_path, UTF_8);
}

Object *parser_generic::parseISO_8859_15(const std::string &file_path){
	return parser_generic::parse(file_path, ISO_8859_15);
}

Object *parser_generic::parse(const std::string &file_path, Encoding file_encoding){
	using namespace std;
		
	LOG(LogLevel::Debug) << "parsing file " << file_path;	

	//open the file as a wide character stream
	basic_ifstream<wchar_t> input{file_path};

	//unset skipping of whitespace
	input.unsetf(std::ios::skipws);

	//create a new local based on the old one, but using the codecvt facet to decode the file encoding to the system dependant WCHAR encoding
	locale loc{input.getloc(), new ConversionFacet<wchar_t,char>{file_encoding, WCHAR}};
	
	/*
	input will now interpret the file as encoded by the specified encoding and transform the output into wide characters using the system dependentencoding
	this will be UCS (4 byte codepoints) in most linux distro's and UTF-16 LE on windows
	because the system encoding is not specifically named, the parser is portable if the ConversionFacet codecvt facet is implemented on all target systems
	*/
	input.imbue(loc);

	if(!input){
		LOG(LogLevel::Error) << "unable to open input stream to path " << file_path;
		return nullptr;
	}
	
	//delegate to parse, which won't need to care about the encoding anymore 
	return parser_generic::parse(input);	
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
	
	return do_parse(begin, end);
}

/*
	Helper functions to pretty print logs
	TODO: remove these
*/
struct print_str{
        
        std::wstring value_;

        explicit print_str(const std::wstring &value) : value_(value){};

};

std::ostream &operator<<(std::ostream &output, const print_str &str){
        for(wchar_t c : str.value_){
                if(c < 128){
                        output.put(c);
                }else{
                        output << '<' << static_cast<int>(c) << '>';
                }
        }
        return output;
};

/*
	An exception type for use in the parser to simplify control flow on errors
*/
class ParseError : public std::runtime_error{
public:
        ParseError() : std::runtime_error{"unspecified parse error"}{};

        ParseError(const std::string &msg) : std::runtime_error{msg}{};

};

/*
	Parser state contains all state information for the parser's functions
	This allows for a compatibility layer to easily bind to Object's data members without having to change the Object interface and also to make the parser thread safe
*/
class ParserState{
private:
        Object *root_;
        std::stack<Object *> stack_;
        std::wstring identifier_;

        Object &current_scope(){
                if(stack_.empty()){
                        throw ParseError{"no current scope defined: stack is empty"};
                }
                return *stack_.top();
        };

	std::string encode(const std::wstring &str){
		return Utils::convertToUTF8(str);
	};

        std::string read_and_discard_identifier(){
                if(identifier_.empty()){
                        throw ParseError{"no identifier specified"};
                }
		//TODO: to much copying, do this better with move operations
                std::wstring result;
                std::swap(result, identifier_);
                return encode(result);
        };
public:

        ParserState() : root_(new Object{"topLevel"}), stack_{}{
                stack_.push(root_);
        };

        void set_identifier(const std::wstring &identifier){
                identifier_ = identifier;
        };

        void set_string_value(const std::wstring &literal){
                current_scope().setValue(encode(literal));
        };

        void add_to_map(const std::wstring &value){
		Object* object = new Object{read_and_discard_identifier()};
		object->setValue(encode(value));
                current_scope().setValue(object);
        };
        void add_to_list(const std::wstring &value){
                current_scope().addToList(encode(value));
        };

        void enter_scope(wchar_t){
                Object *object;
                if(identifier_.empty()){
                        object = new Object{"listItem"}; //TODO: maybe add an identifier here
                        current_scope().setObjList(true);
			current_scope().addObject(object);
                }else{
                        object = new Object{read_and_discard_identifier()};
                        current_scope().setValue(object);
                }
                stack_.push(object);
        };

        void leave_scope(wchar_t){
		if(stack_.empty()){
                        throw ParseError{"no parent scope found: stack is empty"};
                }
                stack_.pop();
        };

        void comment(const std::wstring &str){
        };

        Object *consume_result(){
                Object *result = nullptr;
                std::swap(result, root_);
                return result;
        };

        ~ParserState(){
                delete root_;
        };
};

/*
	First implementation of the parser
*/

namespace qi = boost::spirit::qi;

namespace parsers = qi::standard_wide;

const wchar_t QUOTE = L'"';

const wchar_t ASSIGN = L'=';

const wchar_t SCOPE_START = L'{';

const wchar_t SCOPE_END = L'}';

const wchar_t COMMENT = L'#';

const std::wstring IDENTIFIER_NOT_ALLOWED{L"{=#}\""};

template<typename Iterator> class Grammar : public qi::grammar<Iterator>{
private:

        ParserState *state_;

        qi::rule<Iterator, std::wstring()> comment_;

        qi::rule<Iterator> white_space_;

        qi::rule<Iterator, std::wstring()> quoted_string_;

        qi::rule<Iterator, wchar_t()> identifier_character_;

        qi::rule<Iterator, std::wstring()> unquoted_string_;

        qi::rule<Iterator> object_;

        qi::rule<Iterator> key_;

        qi::rule<Iterator> value_;

        qi::rule<Iterator> key_value_sequence_;

        qi::rule<Iterator> string_list_;

        qi::rule<Iterator> object_list_;

        qi::rule<Iterator> statement_sequence_;

public:
        Grammar(ParserState *state) : Grammar::base_type(statement_sequence_), state_(state){
                using parsers::char_;
                using parsers::space;
                using boost::spirit::eol;

                //WARNING: all character literals must be expressed as wide literals L"x" or explicitly cast to wchar_t
                //Any literals using the char format 'x' will not be portable

                comment_ = char_(COMMENT) >> *(char_ - eol);

                white_space_ = *(space | comment_[bind(&ParserState::comment, state_, _1)]);

                quoted_string_ = QUOTE >> *(char_ - QUOTE) >> QUOTE;

                identifier_character_ = char_ - (char_(IDENTIFIER_NOT_ALLOWED) | space);

                unquoted_string_ = (+identifier_character_);

                key_ = (quoted_string_ | unquoted_string_)[bind(&ParserState::set_identifier, state_, _1)];

                value_ = (((quoted_string_ | unquoted_string_)[bind(&ParserState::add_to_map, state_,_1)]) | object_);

                key_value_sequence_ = +(key_ >> white_space_ >> ASSIGN >> white_space_ >> value_ >> white_space_);

                string_list_ = +((quoted_string_ | unquoted_string_ )[bind(&ParserState::add_to_list,state_, _1)] >> white_space_);

                object_list_ = +(object_ >> white_space_);

                object_ = char_(SCOPE_START)[bind(&ParserState::enter_scope, state_, _1)] >> white_space_ >> statement_sequence_  >> char_(SCOPE_END)[bind(&ParserState::leave_scope, state_, _1)];

                statement_sequence_ = white_space_ >> (key_value_sequence_ | object_list_ | string_list_ | white_space_) >> white_space_;
        };
};

template<typename Iterator> Object *do_parse(Iterator begin, Iterator end){
	using boost::spirit::qi::parse;
	using namespace std;
	ParserState state;
	Grammar<Iterator> grammar{&state};
	try{
		bool result = parse(begin, end, grammar);
		if(result){
			return state.consume_result();
		}else{
			LOG(LogLevel::Error) << "parsing errors during parsing of stream";
		}
	}catch(ParseError &e){
		LOG(LogLevel::Error) << "parsing errors during parsing of stream: " << e.what();
	}
	return nullptr;;
};
