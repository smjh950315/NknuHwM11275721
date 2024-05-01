#pragma once
#include "neuron.hpp"
namespace nknuse::ai::component {
	class NeuronPool {
		using This = NeuronPool;
		static constexpr void _Functor_AdjustWeight(nfloat& _weight, const nfloat _input, nfloat _errRate) {
			_weight += (nfloat)_input * _errRate;
		}

		static void _Log_Weight_Difference(nfloat pre, nfloat now, size_t& counter, std::string& str) {
			auto rpre = round(pre * 10);
			auto rnow = round(now * 10);
			if (rpre == rnow)
				AppendString(str, "\nweight output ", counter, " : ", pre);
			else {
				std::string diffStr = rnow > rpre ? "+" : "";
				diffStr += std::to_string((rnow - rpre) / 10);
				AppendString(str, "\nweight output ", counter, " : ", pre, " -> ", now, "(", diffStr, ")");
			}
			counter++;
		}
		static void _Init_IfNeed(NeuronPool* _pool) {
			if (!_pool) { return; }
			if (!_pool->_LearningRate.get()) {
				_pool->_LearningRate = std::make_shared<nfloat>();
			}
			if (!_pool->_Threshold.get()) {
				_pool->_Threshold = std::make_shared<nfloat>();
			}
		}
		// 將輸入的浮點向量內的浮點數賦予隨機值
		static void _Rand_Vec_Init(Vec<nfloat>* _ptrVec, nfloat _randMin, nfloat _randMax, size_t _decimal) {
			if (!_ptrVec) { return; }
			auto& vals = *_ptrVec;
			for (auto& val : vals) {
				val = nknuse::common::Numeric::Random(_randMin, _randMax, _decimal);
			}
		}
		// 調整權重
		static void _Weight_Adjust(NeuronPool* _mainPool, Neuron* _ptrNeuron, const DataView<nfloat>& _inputs, nfloat _errValue, std::string& _str) {
			if (fabs(_errValue) > _mainPool->Epsilon) {
				auto lRate = _mainPool->LearningRate();
				auto& weights = _ptrNeuron->_Weights;
				nknuse::common::CalcDualIteration(weights, _inputs, _Functor_AdjustWeight, _errValue * lRate);
				if (_mainPool->FuncWriteWeights) {
					_mainPool->FuncWriteWeights(_ptrNeuron, _str);
				}
			}
			else if (_mainPool->FuncWriteWeights) {
				_mainPool->FuncWriteWeights(_ptrNeuron, _str);
			}
		}
		static void _Run_Epoch(NeuronPool* _mainPool, Neuron* _ptrNeuron, Vec<TrainingData>* _trainData, std::string& strDisplay) {
			_THROW_EX_NULLPTR(_ptrNeuron);
			_THROW_EX_NULLPTR(_mainPool);
			_THROW_EX_NULLPTR(_trainData);
			_THROW_EX_NULLPTR(_mainPool->FuncGetErrorValue);
			auto& input = *_trainData;
			Vec<nfloat> prevWeight;
			auto weightView = _ptrNeuron->GetWeightView();
			for (auto& weight : weightView) {
				prevWeight.push_back(weight);
			}
			size_t counter{};
			for (auto& i : input) {
				AppendString(strDisplay, "Run Input : ", counter, "\n");
				if (_mainPool->FuncWriteWeightStrWithInput) {
					_mainPool->FuncWriteWeightStrWithInput(_ptrNeuron, i.GetInputView(), strDisplay);
				}
				auto neuronOut = _ptrNeuron->GetResult(i.GetInputView(), strDisplay);
				auto errorValue = _mainPool->FuncGetErrorValue(i.Answer - neuronOut);

				AppendString(strDisplay, "Answer : ", i.Answer, "\n");
				if (_mainPool->FuncWriteErrorValueStr) {
					_mainPool->FuncWriteErrorValueStr(errorValue, strDisplay);
				}
				_Weight_Adjust(_mainPool, _ptrNeuron, i.GetInputView(), errorValue, strDisplay);
				counter++;
			}
			auto weightView2 = _ptrNeuron->GetWeightView();
			size_t counterW{};
			common::CalcDualIteration(prevWeight, weightView2, _Log_Weight_Difference, counterW, strDisplay);
		}
		// 閥值
		SharedValueF _Threshold{};
		// 學習率
		SharedValueF _LearningRate{};
		// 神經元的群集
		Vec<Neuron> _Neurons{};
	public:
		nfloat& Threshold() {
			if (!this->_Threshold.get()) {
				this->_Threshold = std::make_shared<nfloat>();
			}
			return *this->_Threshold;
		}
		nfloat& LearningRate() {
			if (!this->_LearningRate.get()) {
				this->_LearningRate = std::make_shared<nfloat>();
			}
			return *this->_LearningRate;
		}
		nfloat Epsilon = 0.05;
		nfloat MaxWeightInit{};
		nfloat MinWeightInit{};
		FnSetResultF FuncSetResult{};
		FnGetErrorValue FuncGetErrorValue{};
		FnWriteWeightStrWithInput FuncWriteWeightStrWithInput{};
		FnWriteWeightStr FuncWriteWeights{};
		FnWriteErrorVauleStr FuncWriteErrorValueStr{};
		NeuronPool(nfloat _minWeightInit, nfloat _maxWeightInit) : MinWeightInit(_minWeightInit), MaxWeightInit(_maxWeightInit) {

		}
		// 跑一輪 Epoch
		void RunEpoch(Vec<TrainingData>& _trainData) {
			std::string prnt{};
			for (auto& neu : this->_Neurons) {
				_Run_Epoch(this, &neu, &_trainData, prnt);
			}
			std::cout << prnt;
		}
		void RunEpoch(Vec<TrainingData>& _trainData, std::string& _out) {
			for (auto& neu : this->_Neurons) {
				_Run_Epoch(this, &neu, &_trainData, _out);
			}
		}
		// 新增神經元，參數是輸入的數量
		void AddNeuron(size_t countOfInputs) {
			_Init_IfNeed(this);
			this->_Neurons.push_back(Neuron(this->_Threshold, this->_LearningRate, this->FuncSetResult));
			auto ptr = this->_Neurons.rbegin();
			ptr->_Weights.resize(countOfInputs);
			This::_Rand_Vec_Init(&ptr->_Weights, this->MinWeightInit, this->MaxWeightInit, 1);
		}
	};
}
