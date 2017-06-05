#include "Encoding.h"

using namespace parser_generic;

// Use static variables inside functions to make sure only one 'instance' of each encoding is created and that it lives on until program shutdown

Encoding create_ascii(){
        static EncodingDescriptor encoding{"ASCII", false, 1};
        return &encoding;
};

Encoding create_utf_8(){
        static EncodingDescriptor encoding{"UTF-8", true, 4};
        return &encoding;
};

Encoding create_utf_16_le(){
        static EncodingDescriptor encoding{"UTF-16LE", true, 4};
        return &encoding;
};

Encoding create_iso_8859_15(){
        static EncodingDescriptor encoding{"ISO-8859-15", false, 1};
        return &encoding;
};

Encoding create_ucs(){
        static EncodingDescriptor encoding{"UCS-4-INTERNAL", false, 4};
        return &encoding;
};

Encoding create_wchar_t(){
        static EncodingDescriptor encoding{"wchar_t", false, sizeof(wchar_t)};
        return &encoding;
};

const Encoding parser_generic::ASCII{create_ascii()};
const Encoding parser_generic::UTF_8{create_utf_8()};
const Encoding parser_generic::UTF_16_LE{create_utf_16_le()};
const Encoding parser_generic::ISO_8859_15{create_iso_8859_15()};
const Encoding parser_generic::WCHAR{create_wchar_t()};
const Encoding parser_generic::UCS{create_ucs()};
