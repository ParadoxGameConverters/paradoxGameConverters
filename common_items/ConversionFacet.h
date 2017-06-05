#ifndef CONVERSION_FACET_H
#define CONVERSION_FACET_H

#include <locale>

#include "ConversionFacetUtils.h"
#include "Encoding.h"

namespace parser_generic{
	
	/*
	* An implementation of the codecvt locale facet that converts between the supplied encodings and character types
	* Warning: Most C++ standard library implementations only provide those specializations of codecvt_base required by the standard
	* To be sure, only use the following specializations for ConversionFacet:
	*  <char, char, mbstate_t> 
	*  <wchar_t, char, mbstate_t> and <char, wchar_t, mbstate_t>
	*  <char, char16_t, mbstate_t> and <char16_t, char, mbstate_t> (c++11 only)
	*  <char, char32_t, mbstate_t> and <char32_t, char, mbstate_t> (c++11 only)
	*
	* If using this outside the context of a locale, remember to call unshift on each state used before deallocation of the facet to make sure no dangling state pointers are left behind
	*/
	template<typename InChar, typename ExChar, typename State = std::mbstate_t> class ConversionFacet : public std::codecvt<InChar, ExChar, State>{
        public:
                using Base = std::codecvt<InChar,ExChar, State>;
                using result = typename Base::result;
        private:

                using Conversions = conversion_facet_utils::Conversions;	
		using StateManager = conversion_facet_utils::StateManager<State>;
		
		mutable StateManager state_manager_;
                bool always_noconv_;
                int encoding_;
                int max_length_;

                static int calc_encoding(Encoding ex){
                        if(ex->variable_length){
                                return 0;
                        }else{
                                return ex->max_character_size / sizeof(ExChar);
                        }
                };

                static int calc_max_length(Encoding in){
                        return in->max_character_size / sizeof(InChar);
                };
	public:

                ConversionFacet(Encoding external_encoding, Encoding internal_encoding) :
                        state_manager_{internal_encoding, external_encoding},
                        always_noconv_{internal_encoding == external_encoding},
                        encoding_{calc_encoding(external_encoding)},
                        max_length_{calc_max_length(internal_encoding)}{};
        protected:

                bool do_always_noconv() const noexcept{
                        return always_noconv_;
                };

                int do_encoding() const noexcept{
                        return encoding_;
                };

                int do_max_length() const noexcept{
                        return max_length_;
                };

                result do_in(State &state, const ExChar *from, const ExChar *from_end, const ExChar * &from_next, InChar *to, InChar *to_end, InChar * &to_next) const{
                        if(always_noconv_){
                                return result::noconv;
                        }
                        Conversions *conversions = state_manager_.acquire(state);
                        return conversions->ex_to_in.convert(from, from_end, from_next, to, to_end, to_next);
                };

                result do_out(State &state, const InChar *from, const InChar *from_end, const InChar *&from_next, ExChar *to, ExChar *to_end, ExChar *&to_next) const{
                        if(always_noconv_){
                                return result::noconv;
                        }
                        Conversions *conversions = state_manager_.acquire(state);
                        return conversions->in_to_ex.convert(from, from_end, from_next, to, to_end, to_next);
                };

                int do_length(State &state, const ExChar *from, const ExChar *from_end, std::size_t max) const{
                        Conversions *conversions = state_manager_.acquire(state);
                        const ExChar *from_next;
                        InChar to[max];
                        InChar *to_next = to;
                        result res = conversions->ex_to_in.convert(from, from_end, from_next, to, to+max, to_next);
                        if(res == Base::ok || res == Base::partial){
                                return static_cast<int>(from_next - from);
                        }else{
                                return max;
                        }
                };

                result do_unshift(State &state, ExChar *to, ExChar *to_limit, ExChar *&to_next) const{
                        to_next = to;
                        Conversions *conversions = state_manager_.acquire(state);
                        state_manager_.release(conversions, state);
                        return Base::ok;
                };

        };

}

#endif
