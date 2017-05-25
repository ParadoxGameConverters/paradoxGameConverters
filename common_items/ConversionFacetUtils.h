
/*
	Do not include this header directly: include ConversionFacet.h instead
	This header may be split into a Windows  and a Linux one should the code become too complicated to easily split
	All OS-specific functions and handles for the codecvt facet are defined here
*/

#ifndef CONVERSION_FACET_UTILS_H
#define CONVERSION_FACET_UTILS_H

#include <locale>
#include <unordered_map>
#include <mutex>
#include <stdexcept>
#include <cwchar>
#include <cstring>

#ifdef __linux__

#include <iconv.h>
#include <errno.h>

#endif

#ifdef _WIN32

//TODO windows includes

#endif

#include "Encoding.h"

namespace parser_generic{
	
	namespace conversion_facet_utils{

#ifdef __linux__
        
                // The Linux specific conversion code
		// This also contains the actual state of the conversion, as codecvt should be stateless
                class Conversion{
                private:
                        iconv_t descriptor_;
                public:
			
			//Does the actual conversion
                        template<typename From, typename To> std::codecvt_base::result convert(const From *from, const From *from_end, const From * &from_next, To *to, To *to_end, To * &to_next) const{
                                using namespace std;
                                size_t from_remainder = static_cast<size_t>(from_end - from) * sizeof(From);
                                size_t to_remainder = static_cast<size_t>(to_end - to) * sizeof(To);

                                from_next = from;
                                to_next = to;
                                size_t conversion_result = iconv(descriptor_, (char **)(&from_next), &from_remainder, (char **)(&to_next),&to_remainder);
                                if(conversion_result == ((size_t)(-1))){
                                        switch(errno){
                                                case EINVAL:
                                                case E2BIG:
                                                        return codecvt_base::partial;
                                                default:        
                                                        return codecvt_base::error;
                                        }
                                }else{
                                        return codecvt_base::ok;
                                }
                        };
        
                        Conversion(Encoding from_encoding, Encoding to_encoding){
                                using namespace std;
                                const char *from_name = from_encoding->name.c_str();
                                const char *to_name = to_encoding->name.c_str();
                                descriptor_ = iconv_open(to_name, from_name);
                                if(descriptor_ == ((iconv_t)(-1))){
                                        throw invalid_argument{std::string{"unsupported conversion: "} + from_name + " -> " + to_name};
                                }
                        };
 
                        ~Conversion(){
                                iconv_close(descriptor_);
                        };

                        Conversion(const Conversion &) = delete;
                        Conversion &operator=(const Conversion &) = delete;
                };              
#endif

#ifdef _WIN32

                //TODO provide a Conversion object for windows
                //This is the only OS specific part of the code

                struct Conversion{
                        Conversion(Encoding, Encoding){};

                        template<typename From, typename To> std::codecvt_base::result convert(const From *from, const From *from_end, const From * &from_next, To *to, To *to_end, To * &to_next) const{
                                return std::codecvt_base::error;
                        };
                };
#endif

        	//a pair of invertible conversions
		struct Conversions{
                        Conversion in_to_ex;
                        Conversion ex_to_in;

                        Conversions(Encoding in, Encoding ex) : in_to_ex{in, ex}, ex_to_in(ex, in){};

                        Conversions(const Conversions &) = delete;
                        Conversions &operator=(const Conversions &) = delete;
                };

		//An object that manages a link between external multi-byte states and an internal map of pointers
		//This is done for two purposes:
		// 1) To avoid exposing raw pointers to the outside world
		// 2) To synchronize multi-threaded access to the facet's state if needed
		//
		template<typename State = std::mbstate_t> class StateManager{
                private:

                        using InternalState = std::size_t;

                        const Encoding internal_encoding_;
                        const Encoding external_encoding_;

                        std::unordered_map<InternalState, Conversions *> data_;
                        std::mutex mutex_;

                        const InternalState empty_state_;
                        InternalState next_state_;

                        // The dirty bit
                        static InternalState to_internal_state(State state){
                                return *reinterpret_cast<InternalState *>(&state);
                        };

                        // even worse
                        static void set_external_state(InternalState in_state, State &state){
                                state = *reinterpret_cast<State *>(&in_state);
                        };

			//apparently the 'recommended' way of resetting mbstate_t
                        static InternalState create_initial_state(){
                                std::mbstate_t state;
                                memset(&state, 0, sizeof(std::mbstate_t));
                                return to_internal_state(state);
                        };

			//gets an existing conversion state pair from the map
                        Conversions *get(InternalState state){
                                using namespace std;
                                lock_guard<mutex> lock{mutex_};
                                auto found = data_.find(state);
                                return found == data_.end() ? nullptr : found->second;
                        };

			//couples a conversion state pair to the internal state provided
                        void put(InternalState state, Conversions *conversions){
                                using namespace std;
                                lock_guard<mutex> lock{mutex_};
                                data_.insert(make_pair(state, conversions));
                        };

			//removes the conversion state pair associated with the internal state flag from the map
			//does NOT delete it
                        void remove(InternalState state){
                                using namespace std;
                                lock_guard<mutex> lock{mutex_};
                                auto found = data_.find(state);
                                if(found != data_.end()){
                                        data_.erase(found);
                                }
                        };

			//generates the next useable state
                        InternalState next_state(){
                                using namespace std;
                                lock_guard<mutex> lock{mutex_};
                                ++next_state_;
                                return next_state_;
                        };

		public:
                        StateManager(Encoding internal_encoding, Encoding external_encoding) : internal_encoding_{internal_encoding}, external_encoding_{external_encoding}, data_{}, mutex_{}, empty_state_{create_initial_state()}, next_state_{empty_state_}{};

			//Acquires a new or existing conversion state pair for the current conversion
                        Conversions *acquire(State &state){
                                InternalState in_state = to_internal_state(state);
                                Conversions *conversions;
                                if(in_state == empty_state_ || !(conversions = get(in_state))){
                                        in_state = next_state();
                                        conversions = new Conversions{internal_encoding_, external_encoding_};
                                        set_external_state(in_state, state);
                                }
                                return conversions;
                        };

			//Releases the conversion state, deleting all state resources and resetting the facet's state flag
                        void release(Conversions *conversions, State &state){
                                remove(to_internal_state(state));
                                set_external_state(empty_state_, state);
                                delete conversions;
                        };

			//Delete all remaining state data
                        ~StateManager(){
                                for(auto pair : data_){
                                        delete pair.second;
                                }
                        };

                        StateManager(const StateManager<State> &) = delete;
                        StateManager<State> &operator=(const StateManager<State> &) = delete;
                };

		
	};

}

#endif
