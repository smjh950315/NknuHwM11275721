#pragma once
#include <random>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#define _THROW_EX_BAD_INPUT throw nknuse::exception::BadInputException();
#define _THROW_EX_NULLPTR(x) if(x == nullptr) {throw nknuse::exception::NullPointerException();}
#define _THROW_EX_BAD_INPUT_IF(x) if(x) {throw nknuse::exception::BadInputException();}
#define CONSTANT(val) static constexpr val
namespace nknuse {
	using f32 = std::float_t;
	using i32 = std::int32_t;
	template<class T>
	using StdPtr = std::shared_ptr<T>;
	template<class T>
	using Vec = std::vector<T>;
#ifdef _WIN64
	using nfloat = std::double_t;
#else
	using nfloat = std::float_t;
#endif // __X64
	template<class T>
	using UnRef = std::remove_reference<T>::type;
	template<class T>
	struct DataView {
		const T* pBegin{};
		const T* pEnd{};
		const T* begin() const { return this->pBegin; }
		const T* end() const { return this->pEnd; }
		size_t size() const { return this->end() - this->begin(); }
		DataView(const T* _ptrBegin, size_t _length) : pBegin(_ptrBegin), pEnd(_ptrBegin + _length) {}
		DataView(const T* _ptrBegin, const T* _ptrEnd) : pBegin(_ptrBegin), pEnd(_ptrEnd) {}
	};
	namespace exception {
		struct NullPointerException {};
		struct BadInputException {};
	};
	namespace common {
		class Numeric {
			static std::random_device RAND_DEVICE;
		public:
			static nfloat Randomf(nfloat _range1, nfloat _range2) {
				if (_range1 == _range2)
					return _range1;
				nfloat min, max;
				if (_range1 < _range2)
					min = _range1, max = _range2;
				else
					min = _range2, max = _range1;
				std::mt19937 _Generator(RAND_DEVICE());
				std::uniform_real_distribution<nfloat> uniNum(min, max);
				nfloat rand = uniNum(_Generator);
				return rand;
			}
			static nfloat Random(nfloat _range1, nfloat _range2, size_t _decimal) {
				auto randVal = Numeric::Randomf(_range1, _range2);
				auto ratio = pow(10, static_cast<nfloat>(_decimal));
				return (round(randVal * ratio)) / ratio;
			}
		};

		class StringUtils {
			using This = StringUtils;
		public:
			template<class T>
			struct is_string_addable {
				template<class U>
				static auto test(int) -> decltype(std::declval<std::string>() + (std::declval<U>()), std::true_type{});
				template<typename>
				static auto test(...) -> std::false_type;
				static constexpr bool value = decltype(test<T>(0))::value;
			};
			template<class T>
			struct is_string_convertable {
				template<class U>
				static auto test(int) -> decltype(std::to_string(std::declval<U>()), std::true_type{});
				template<typename>
				static auto test(...) -> std::false_type;
				static constexpr bool value = decltype(test<T>(0))::value;
			};

			static constexpr void AppendString(std::string& _str) {}

			template<class T, class ... Ts>
			static constexpr void AppendString(std::string& _str, T&& _val, Ts&&..._vals) requires(!This::is_string_addable<T>::value && !is_string_convertable<T>::value) {
				_THROW_EX_BAD_INPUT
			}
			template<class T, class ... Ts>
			static constexpr void AppendString(std::string& _str, T&& _val, Ts&&..._vals) requires(This::is_string_addable<T>::value) {
				_str += (std::forward<T>(_val));
				AppendString(_str, std::forward<Ts>(_vals)...);
			}
			template<class T, class ... Ts>
			static constexpr void AppendString(std::string& _str, T&& _val, Ts&&..._vals) requires(is_string_convertable<T>::value) {
				_str += std::to_string(std::forward<T>(_val));
				AppendString(_str, std::forward<Ts>(_vals)...);
			}
		};

		class PrintUtils {
			using This = PrintUtils;
		public:
			template<typename _Tp>
			struct is_printable {
				template <typename U>
				static auto test(int) -> decltype(std::declval<std::ostream&>() << std::declval<U>(), std::true_type{});

				template<typename>
				static auto test(...) -> std::false_type;

				static constexpr bool value = decltype(test<_Tp>(0))::value;
			};
			using ccstr = const char*;
			static constexpr void _Print_Multiple_Values(ccstr _sep) {}
			template<class T, class ... Ts>
			static constexpr void _Print_Multiple_Values(ccstr _sep, T&& _first, Ts&&... _args) requires (!is_printable<T>::value) {
				_THROW_EX_BAD_INPUT
			}
			template<class T, class ... Ts>
			static constexpr void _Print_Multiple_Values(ccstr _sep, T&& _first, Ts&&... _args) {
				std::cout << _first;
				if (sizeof...(_args)) {
					std::cout << _sep;
				}
				This::_Print_Multiple_Values(_sep, std::forward<Ts>(_args)...);
			}
			template<class T>
			static void _Print_SetValSucceed(const char* _valName, T inputVal) {
				_Print_Multiple_Values("Message: ", _valName, " is been set to ", inputVal);
			}
			static void _Print_IsInvalidInput(const char* _valName) {
				_Print_Multiple_Values("Message: Bad ", _valName, " input!, please try again!\n");
			}
			template<class T>
			static void _Loop_Input_Argument(const char* _argName, bool(*_fnTrySet)(T&, const std::string&), T& _arg) {
				while (true) {
					std::cout << _argName << " : ";
					std::string inputBuffer{};
					std::cin >> inputBuffer;
					if (_fnTrySet(_arg, inputBuffer)) {
						_Print_SetValSucceed(_argName, _arg);
						break;
					}
					_Print_IsInvalidInput(_argName);
				}
			}
		};

		template<class It1, class It2, class _Fn, class..._Args>
		constexpr void CalcDualIteration(It1&& _first, It2&& _second, _Fn _fn, _Args&&..._args) {
			auto iterFirst = _first.begin();
			auto endFirst = _first.end();
			auto iterSecond = _second.begin();
			auto endSecond = _second.end();
			while (iterFirst != endFirst && iterSecond != endSecond) {
				_fn(*iterFirst, *iterSecond, std::forward<_Args>(_args)...);
				iterFirst++; iterSecond++;
			}
		}
	}

	template<class T, class ... Ts>
	constexpr void PrintLine(T&& _first, Ts&&... _args) {
		nknuse::common::PrintUtils::_Print_Multiple_Values("", std::forward<T>(_first), std::forward<Ts>(_args)...);
		std::cout << std::endl;
	}

	template<class T, class ... Ts>
	constexpr void AppendString(std::string& _str, T&& _first, Ts&&... _args) {
		nknuse::common::StringUtils::AppendString(_str, std::forward<T>(_first), std::forward<Ts>(_args)...);
	}

	template<class T>
	constexpr bool TryCastNumber(T& _num,const std::string& _str) {
		try {
			_num = atof(_str.c_str());
			return true;
		}
		catch (...) {			
			return false;
		}		
	}

	template<class T>
	void ArgumentInputLoop(const char* _argName, bool(*_fnTrySet)(T&, const std::string&), T& _arg) {
		nknuse::common::PrintUtils::_Loop_Input_Argument(_argName, _fnTrySet, _arg);
	}
}

