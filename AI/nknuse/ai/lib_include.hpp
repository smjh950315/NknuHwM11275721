#pragma once
#include <nknuse/common.hpp>
namespace nknuse {
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

}