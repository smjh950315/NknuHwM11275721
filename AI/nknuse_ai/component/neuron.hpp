#pragma once
#include <nknuse_ai/lib_include.hpp>
namespace nknuse::ai::component {
	class Neuron {
		using This = Neuron;
		friend class NeuronPool;
		Weights _Weights{};
		SharedValueF _Threshold{};
		SharedValueF _LearningRate{};
		FnSetResultF _FuncSetResult{};
		Neuron(SharedValueF _th, SharedValueF _lr, FnSetResultF _fnCalc) :_Threshold(_th), _LearningRate(_lr), _FuncSetResult(_fnCalc) {}
	public:
		static void CalcTotal(nfloat _weight, nfloat _input, nfloat& _resOutput) {
			_resOutput += _weight * _input;
		}
		Neuron(const Neuron&) = delete;
		Neuron& operator=(const Neuron&) = delete;
		Neuron(Neuron&&) = default;
		Neuron& operator=(Neuron&&) = default;
		DataView<nfloat> GetWeightView() const {
			return DataView<nfloat>(_Weights.data(), _Weights.size());
		}
		nfloat GetResult(const DataView<nfloat>& _inputs, std::string& _str) const {
			_THROW_EX_BAD_INPUT_IF(_inputs.size() == 0);
			nfloat resVal{};
			nknuse::common::CalcDualIteration(GetWeightView(), _inputs, CalcTotal, resVal);
			resVal = _FuncSetResult(resVal, *this->_Threshold);
			_str += "Neuron : ";
			_str += std::to_string(resVal);
			_str += '\n';
			return resVal;
		}
	};
};