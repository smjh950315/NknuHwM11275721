#pragma once
#include "neuron.hpp"
namespace nknuse::ai::component {
	class NeuronPool {
		using This = NeuronPool;
		static constexpr void _Functor_AdjustWeight(nfloat& _weight, const nfloat _input, nfloat _errRate) {
			_weight += (nfloat)_input * _errRate;
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
		static void _Rand_Vec_Init(Vec<nfloat>* _ptrVec, nfloat _randMin, nfloat _randMax, size_t _decimal) {
			if (!_ptrVec) { return; }
			auto& vals = *_ptrVec;
			for (auto& val : vals) {
				val = nknuse::common::Numeric::Random(_randMin, _randMax, _decimal);
			}
		}
		static void _Weight_Adjust(NeuronPool* _mainPool, Neuron* _ptrNeuron, const DataView<nfloat>& _inputs, nfloat _errValue, std::string& _str) {
			if (fabs(_errValue) > _mainPool->Epsilon) {
				auto lRate = _mainPool->LearningRate();
				auto inputBeg = _inputs.begin();
				auto inputEnd = _inputs.end() - 1;
				auto& weights = _ptrNeuron->_Weights;
				nknuse::common::CalcDualIteration(weights, _inputs, _Functor_AdjustWeight, _errValue * lRate);
				if (_mainPool->FuncWriteWeights) {
					_mainPool->FuncWriteWeights(_ptrNeuron, _str);
				}
			}
			std::string str{};
			if (_mainPool->FuncWriteWeights) {
				_mainPool->FuncWriteWeights(_ptrNeuron, str);
			}
			std::cout << str;
		}
		static void _Run_Epoch(NeuronPool* _mainPool, Neuron* _ptrNeuron, Vec<TrainingData>* _trainData, std::string& strDisplay) {
			_THROW_EX_NULLPTR(_mainPool->FuncGetErrorValue);
			auto& input = *_trainData;
			size_t counter{};
			for (auto& i : input) {
				strDisplay += "Run Input : ";
				strDisplay += std::to_string(counter);
				strDisplay += "\n";
				if (_mainPool->FuncWriteWeightStrWithInput) {
					_mainPool->FuncWriteWeightStrWithInput(_ptrNeuron, i.GetInputView(), strDisplay);
				}
				auto neuronOut = _ptrNeuron->GetResult(i.GetInputView(), strDisplay);
				auto errorValue = _mainPool->FuncGetErrorValue(i.Answer - neuronOut);

				strDisplay += "Answer : ";
				strDisplay += std::to_string(i.Answer);
				strDisplay += "\n";
				if (_mainPool->FuncWriteErrorValueStr) {
					_mainPool->FuncWriteErrorValueStr(errorValue, strDisplay);
				}
				_Weight_Adjust(_mainPool, _ptrNeuron, i.GetInputView(), errorValue, strDisplay);
				counter++;
				strDisplay += "   \n";
				std::cout << strDisplay;
			}
		}
		SharedValueF _Threshold{};
		SharedValueF _LearningRate{};
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
		// 禲近 Epoch
		void RunEpoch(Vec<TrainingData>& _trainData) {
			std::string prnt{};
			for (auto& neu : this->_Neurons) {
				_Run_Epoch(this, &neu, &_trainData, prnt);
			}
			std::cout << prnt;
		}
		// 穝糤竒じ把计琌块计秖
		void AddNeuron(size_t countOfInputs) {
			_Init_IfNeed(this);
			_Neurons.push_back(Neuron(this->_Threshold, this->_LearningRate, this->FuncSetResult));
			auto ptr = _Neurons.rbegin();
			ptr->_Weights.resize(countOfInputs);
			This::_Rand_Vec_Init(&ptr->_Weights, MinWeightInit, MaxWeightInit, 1);
		}
	};
}
