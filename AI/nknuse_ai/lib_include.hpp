#pragma once
#include <random>
#include <memory>
#include <string>
#include <iostream>
#define _THROW_EX_BAD_INPUT throw nknuse::exception::BadInputException();
#define _THROW_EX_NULLPTR(x) if(x == nullptr) {throw nknuse::exception::NullPointerException();}
#define _THROW_EX_BAD_INPUT_IF(x) if(x) {throw nknuse::exception::BadInputException();}
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
		const T* begin() const { return pBegin; }
		const T* end() const { return pEnd; }
		size_t size() const {
			return end() - begin();
		}
		DataView(const T* _ptrBegin, size_t _length) : pBegin(_ptrBegin), pEnd(_ptrBegin + _length) {

		}
		DataView(const T* _ptrBegin, const T* _ptrEnd) : pBegin(_ptrBegin), pEnd(_ptrEnd) {

		}
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

	namespace ai {
		struct TrainingData {
			// 輸入實際儲存的容器
			Vec<nfloat> _Inputs{};
			constexpr void _SetInputs() {}
			template<class T, class... Ts>
			constexpr void _SetInputs(T&& val1, Ts&&...vals) {
				this->_Inputs.push_back(std::forward<T>(val1));
				this->_SetInputs(std::forward<Ts>(vals)...);
			}			
		public:

			TrainingData(const TrainingData& _other) {
				this->_Inputs = _other._Inputs;
				this->Answer = _other.Answer;
			}
			TrainingData(TrainingData&& _other) noexcept {
				this->_Inputs = std::move( _other._Inputs);
				this->Answer = _other.Answer;
			}
			TrainingData& operator=(const TrainingData& _other) {
				this->_Inputs = _other._Inputs;
				this->Answer = _other.Answer;
				return *this;
			}
			TrainingData& operator=(TrainingData&& _other) noexcept {
				this->_Inputs = std::move(_other._Inputs);
				this->Answer = _other.Answer;
				return *this;
			}

			// 解答
			nfloat Answer{};
			TrainingData() {}
			// 建構: 第一個參數是預期結果，第二個以後的參數是輸入
			template<class T, class... Ts>
			TrainingData(T _answer, Ts&&... vals) : Answer((nfloat)_answer) {
				this->SetInputs(std::forward<Ts>(vals)...);
			}
			// 取得輸入值
			DataView<nfloat> GetInputView() const {
				return DataView(_Inputs.data(), _Inputs.size());
			}
			// 設定輸入值
			template<class... Ts>
			constexpr void SetInputs(Ts&&... vals) {
				this->_SetInputs(std::forward<Ts>(vals)...);
			}
			// 取得資料指標
			constexpr const nfloat* Data() const {
				return this->_Inputs.data();
			}
			// 取得資料數
			constexpr size_t Count()const {
				return this->_Inputs.size();
			}
			// 實作 C++ 迭代容器的介面
			constexpr auto begin() const {
				return this->_Inputs.begin();
			}
			// 實作 C++ 迭代容器的介面
			constexpr auto end() const {
				return this->_Inputs.end();
			}
		};
		namespace component {
			using Weights = std::vector<nfloat>;
			using SharedValueF = StdPtr<nfloat>;
			using FnSetResultF = nfloat(*)(nfloat, nfloat);
			using FnGetErrorValue = nfloat(*)(nfloat);
	
			class Neuron;
			class NeuronPool;

			using FnWriteErrorVauleStr = void(*)(nfloat, std::string&);
			using FnWriteWeightStr = void(*)(Neuron*, std::string&);
			using FnWriteWeightStrWithInput = void(*)(Neuron*, const DataView<nfloat>&, std::string&);
		}
	};
	template<class T, class ... Ts>
	constexpr void PrintLine(T&& _first, Ts&&... _args) {
		nknuse::common::PrintUtils::_Print_Multiple_Values("", std::forward<T>(_first), std::forward<Ts>(_args)...);
		std::cout << std::endl;
	}
}