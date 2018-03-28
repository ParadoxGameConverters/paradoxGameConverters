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

#include <Windows.h>

#endif

#include "Encoding.h"

namespace parser_generic {

	namespace conversion_facet_utils {

#ifdef __linux__

		// The Linux specific conversion code
		// This also contains the actual state of the conversion, as codecvt should be stateless
		class Conversion {
		private:
			iconv_t descriptor_;
		public:

			//Does the actual conversion
			template<typename From, typename To> std::codecvt_base::result convert(const From *from, const From *from_end, const From * &from_next, To *to, To *to_end, To * &to_next) const {
				using namespace std;
				size_t from_remainder = static_cast<size_t>(from_end - from) * sizeof(From);
				size_t to_remainder = static_cast<size_t>(to_end - to) * sizeof(To);

				from_next = from;
				to_next = to;
				size_t conversion_result = iconv(descriptor_, (char **)(&from_next), &from_remainder, (char **)(&to_next), &to_remainder);
				if (conversion_result == ((size_t)(-1))) {
					switch (errno) {
					case EINVAL:
					case E2BIG:
						return codecvt_base::partial;
					default:
						return codecvt_base::error;
					}
				}
				else {
					return codecvt_base::ok;
				}
			};

			Conversion(Encoding from_encoding, Encoding to_encoding) {
				using namespace std;
				const char *from_name = from_encoding->name.c_str();
				const char *to_name = to_encoding->name.c_str();
				descriptor_ = iconv_open(to_name, from_name);
				if (descriptor_ == ((iconv_t)(-1))) {
					throw invalid_argument{ std::string{ "unsupported conversion: " } +from_name + " -> " + to_name };
				}
			};

			~Conversion() {
				iconv_close(descriptor_);
			};

			Conversion(const Conversion &) = delete;
			Conversion &operator=(const Conversion &) = delete;
		};
#endif

#ifdef _WIN32

		class ConversionStrategy {
		public:

			virtual std::codecvt_base::result convert(const char *from, const char *from_end, const char *&from_next, wchar_t *to, wchar_t *to_end, wchar_t *&to_next) = 0;

			virtual std::codecvt_base::result convert(const wchar_t *from, const wchar_t *from_end, const wchar_t *&from_next, char *to, char *to_end, char *&to_next) = 0;

			virtual ~ConversionStrategy() {};

		protected:
			ConversionStrategy() {};

		private:
			ConversionStrategy(const ConversionStrategy &) = delete;
			ConversionStrategy &operator=(const ConversionStrategy &) = delete;
		};

		class UTF8ToWCharStrategy : public ConversionStrategy {
		private:
			static const unsigned int code_page_{ 65001 };

			int calculate_char_size(unsigned int leading_octet) {
				if ((leading_octet & 0x80) == 0) {
					return 1;
				}
				else if ((leading_octet & 0x40) == 0) {
					//invalid leading octet
					return -1;
				}
				else if ((leading_octet & 0x20) == 0) {
					return 2;
				}
				else if ((leading_octet & 0x10) == 0){
					return 3;
				}
				else if((leading_octet & 0x08) == 0){
					return 4;
				}
				else {
					//big trouble: characters of more than 4 bytes won't be represented correctly on some systems where sizeof(wchar_t) < 4 => error
					return -2;
				}
			};

			// Note: we're doing the most basic check possible: no overlong encoding or illegal code point checks
			bool check_trailing_octet(unsigned int trailing_octet) {
				return (trailing_octet & 0xC0) == 0x80;
			};

			int calculate_safe_string_size(const char *begin, const char *end, int output_buffer_size) {
				int safe_size = 0;
				while (true) {
					if (begin == end || output_buffer_size == 0) {
						return safe_size;
					}
					else {
						int char_size = calculate_char_size(*begin);
						if (char_size < 0) {
							return char_size;
						}
						else {
							++begin;
							for (int i = 1; i < char_size; ++i) {
								if (begin == end) {
									return safe_size;
								}
								else {
									if (!check_trailing_octet(*begin)) {
										return -3;
									}
								}
								++begin;
							}
							safe_size += char_size;
							--output_buffer_size;
						}
					}
				}
			};
		
		public:
			UTF8ToWCharStrategy() {}

			std::codecvt_base::result convert(const char *from, const char *from_end, const char *&from_next, wchar_t *to, wchar_t *to_end, wchar_t *&to_next) {
				int from_size = static_cast<int>(from_end - from);
				int to_size = static_cast<int>(to_end - to);
				
				int safe_from_size = calculate_safe_string_size(from, from_end, to_size);
				if (safe_from_size < 0) {
					return std::codecvt_base::error;
				}

				if (safe_from_size == 0) {
					from_next = from;
					to_next = to;
					if (from_size == 0) {
						return std::codecvt_base::ok;
					}
					else {
						return std::codecvt_base::partial;
					}
				}
				else {
					int result = MultiByteToWideChar(code_page_, MB_ERR_INVALID_CHARS, from, safe_from_size, to, to_size);
					if (result == 0) {
						return std::codecvt_base::error;
					}
					else {
						from_next = from + safe_from_size;
						to_next = to + result;
						if (safe_from_size < from_size) {
							return std::codecvt_base::partial;
						}
						else {
							return std::codecvt_base::ok;
						}
					}
				}
			};

			std::codecvt_base::result convert(const wchar_t *from, const wchar_t *from_end, const wchar_t *&from_next, char *to, char *to_end, char *&to_next) {
				int from_size = static_cast<int>(from_end - from);
				int to_size = static_cast<int>(to_end - to);

				int expected_to_size = WideCharToMultiByte(code_page_, WC_ERR_INVALID_CHARS, from, from_size, NULL, 0, NULL, false);
				int safe_from_size;
				if (expected_to_size == 0) {
					return std::codecvt_base::error;
				}else if(expected_to_size <= to_size) {
					safe_from_size = from_size;
				}
				else {
					//assume worst case: 1 wchar_t encoded character needs 4 octets to encode in UTF-8
					safe_from_size = to_size / 4;
				}
				
				if (safe_from_size == 0) {
					from_next = from;
					to_next = to;
					if (from_size == 0) {
						return std::codecvt_base::ok;
					}
					else {
						return std::codecvt_base::partial;
					}
				}

				int result = WideCharToMultiByte(code_page_, WC_ERR_INVALID_CHARS, from, safe_from_size, to, to_size, NULL, false);
				if (result == 0) {
					return std::codecvt_base::error;
				}
				else {
					from_next = from + safe_from_size;
					to_next = to + result;
					if (safe_from_size == from_size) {
						return std::codecvt_base::ok;
					}
					else {
						return std::codecvt_base::partial;
					}
				}
				
			};
		};

		class ISO8865_15ToWCharStrategy : public ConversionStrategy {
		private:
			static const unsigned int code_page_{ 28605 };
		public:
			ISO8865_15ToWCharStrategy() {};

			std::codecvt_base::result convert(const char *from, const char *from_end, const char *&from_next, wchar_t *to, wchar_t *to_end, wchar_t *&to_next) {
				int from_size = static_cast<int>(from_end - from);
				int to_size = static_cast<int>(to_end - to);
				int size = from_size > to_size ? to_size : from_size;
				int result = MultiByteToWideChar(code_page_, MB_ERR_INVALID_CHARS, from, size, to, size);
				if (result == 0) {
					return std::codecvt_base::error;
				}else {
					from_next = from + result;
					to_next = to + result;
					if (size < from_size) {
						return std::codecvt_base::partial;
					}
					else {
						return std::codecvt_base::ok;
					}
				}
			};

			std::codecvt_base::result convert(const wchar_t *from, const wchar_t *from_end, const wchar_t *&from_next, char *to, char *to_end, char *&to_next) {
				int from_size = static_cast<int>(from_end - from);
				int to_size = static_cast<int>(to_end - to);
				int size = from_size > to_size ? to_size : from_size;
				int result = WideCharToMultiByte(code_page_, WC_ERR_INVALID_CHARS, from, size, to, size, NULL, false);
				if (result == 0) {
					return std::codecvt_base::error;
				}
				else {
					from_next = from + result;
					to_next = to + result;
					if (size < from_size) {
						return std::codecvt_base::partial;
					}
					else {
						return std::codecvt_base::ok;
					}
				}
			};
		};

		class Conversion {
		private:
			ConversionStrategy * const strategy_;

			static ConversionStrategy *create_conversion_strategy(Encoding from, Encoding to) {
				using namespace std;
				if ((from == ISO_8859_15 && to == WCHAR) || (from == WCHAR && to == ISO_8859_15)) {
					return new ISO8865_15ToWCharStrategy{};
				}
				else if ((from == UTF_8 && to == WCHAR) || (from == WCHAR && to == UTF_8)) {
					return new UTF8ToWCharStrategy{};
				}
				else {
					throw invalid_argument{ string{ "unsupported conversion: " }+from->name + " -> " + to->name };
				}
			};
		public:
			Conversion(Encoding from, Encoding to) : strategy_{create_conversion_strategy(from, to)} {};

			template<typename From, typename To> std::codecvt_base::result convert(const From *from, const From *from_end, const From * &from_next, To *to, To *to_end, To * &to_next) {
				return std::codecvt_base::error;
			};

			template<> std::codecvt_base::result convert<char, wchar_t>(const char *from, const char *from_end, const char * &from_next, wchar_t *to, wchar_t *to_end, wchar_t * &to_next) {
				return strategy_->convert(from, from_end, from_next, to, to_end, to_next);
			};

			template<> std::codecvt_base::result convert<wchar_t, char>(const wchar_t *from, const wchar_t *from_end, const wchar_t * &from_next, char *to, char *to_end, char * &to_next) {
				return strategy_->convert(from, from_end, from_next, to, to_end, to_next);
			};
		};

#endif

		//a pair of invertible conversions
		struct Conversions {
			Conversion in_to_ex;
			Conversion ex_to_in;

			Conversions(Encoding in, Encoding ex) : in_to_ex{ in, ex }, ex_to_in(ex, in){};

			Conversions(const Conversions &) = delete;
			Conversions &operator=(const Conversions &) = delete;
		};

		//An object that manages a link between external multi-byte states and an internal map of pointers
		//This is done for two purposes:
		// 1) To avoid exposing raw pointers to the outside world
		// 2) To synchronize multi-threaded access to the facet's state if needed
		//
		template<typename State = std::mbstate_t> class StateManager {
		private:

			using InternalState = std::size_t;

			const Encoding internal_encoding_;
			const Encoding external_encoding_;

			std::unordered_map<InternalState, Conversions *> data_;
			std::mutex mutex_;

			const InternalState empty_state_;
			InternalState next_state_;

			// The dirty bit
			static InternalState to_internal_state(State state) {
				return *reinterpret_cast<InternalState *>(&state);
			};

			// even worse
			static void set_external_state(InternalState in_state, State &state) {
				state = *reinterpret_cast<State *>(&in_state);
			};

			//apparently the 'recommended' way of resetting mbstate_t
			static InternalState create_initial_state() {
				std::mbstate_t state;
				memset(&state, 0, sizeof(std::mbstate_t));
				return to_internal_state(state);
			};

			//gets an existing conversion state pair from the map
			Conversions *get(InternalState state) {
				using namespace std;
				lock_guard<mutex> lock{ mutex_ };
				auto found = data_.find(state);
				return found == data_.end() ? nullptr : found->second;
			};

			//couples a conversion state pair to the internal state provided
			void put(InternalState state, Conversions *conversions) {
				using namespace std;
				lock_guard<mutex> lock{ mutex_ };
				data_.insert(make_pair(state, conversions));
			};

			//removes the conversion state pair associated with the internal state flag from the map
			//does NOT delete it
			void remove(InternalState state) {
				using namespace std;
				lock_guard<mutex> lock{ mutex_ };
				auto found = data_.find(state);
				if (found != data_.end()) {
					data_.erase(found);
				}
			};

			//generates the next useable state
			InternalState next_state() {
				using namespace std;
				lock_guard<mutex> lock{ mutex_ };
				++next_state_;
				return next_state_;
			};

		public:
			StateManager(Encoding internal_encoding, Encoding external_encoding) : internal_encoding_{ internal_encoding }, external_encoding_{ external_encoding }, data_{}, mutex_{}, empty_state_{ create_initial_state() }, next_state_{ empty_state_ } {};

			//Acquires a new or existing conversion state pair for the current conversion
			Conversions *acquire(State &state) {
				InternalState in_state = to_internal_state(state);
				Conversions *conversions;
				if (in_state == empty_state_ || !(conversions = get(in_state))) {
					in_state = next_state();
					conversions = new Conversions{ internal_encoding_, external_encoding_ };
					set_external_state(in_state, state);
				}
				return conversions;
			};

			//Releases the conversion state, deleting all state resources and resetting the facet's state flag
			void release(Conversions *conversions, State &state) {
				remove(to_internal_state(state));
				set_external_state(empty_state_, state);
				delete conversions;
			};

			//Delete all remaining state data
			~StateManager() {
				for (auto pair : data_) {
					delete pair.second;
				}
			};

			StateManager(const StateManager<State> &) = delete;
			StateManager<State> &operator=(const StateManager<State> &) = delete;
		};


	};

}

#endif
