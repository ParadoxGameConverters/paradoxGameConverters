#ifndef ENCODING_H
#define ENCODING_H

#include <string>

namespace parser_generic{

	
	/*
	* A struct containing the necessary info to determine character length
	* Should never be copied or created, use Encoding instead
	*/        
        struct EncodingDescriptor{
                const std::string name;
                const bool variable_length;
                const std::size_t max_character_size;
                                
                EncodingDescriptor(const std::string &name_, bool variable_length_, std::size_t max_character_size_) : name{name_}, variable_length{variable_length_}, max_character_size{max_character_size_}{};
        };

        //Should be considered an opaque type, use only to tag which encoding to use
	using Encoding = const EncodingDescriptor *;
        
        //7 bit ASCII encoding
        extern const Encoding ASCII;

        //UTF-8 encoding (limited to 4 bytes)
        extern const Encoding UTF_8;

        //ISO 8859 15 encoding
        extern const Encoding ISO_8859_15;

        //UTF-16 little endian encoding
        extern const Encoding UTF_16_LE;

        //native system encoding for wchar_t
        extern const Encoding WCHAR;

        //UCS (4 byte unicode codepoint) currently not used but may be usefull for debugging (code points are well defined values, unlike wchar_t)
        extern const Encoding UCS;
}

#endif
