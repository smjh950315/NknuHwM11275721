#include "nknuse_ai/nknuse_ai.hpp"
using namespace nknuse::ai::component;
using namespace nknuse::ai;
using namespace nknuse;

template<size_t _InputCount>
Vec<TrainingData> GetTrainingDatas() {
	size_t totalCount = pow(2, _InputCount);
	Vec<TrainingData> retVal{};
	retVal.reserve(totalCount);
	for (auto i = 0; i < totalCount; ++i) {
		TrainingData temp{};
		temp._Inputs.resize(_InputCount);
		auto ptr = (nfloat*)temp._Inputs.data();
		for (auto j = 0; j < _InputCount; ++j) {
			ptr[j] = (nfloat)((i & ((int)1 << j)) >> j);
		}
		temp.Answer = i == totalCount - 1 ? 1 : 0;
		retVal.push_back(std::move(temp));
	}
	return retVal;
}

int main() {

#define MIN_VALUE_EPS 0.05
#define DECIMAL_COUNT 3
#define DECIMAL_RATIO (pow(10, DECIMAL_COUNT))
#define SPEC_DECIMAL_VALUE(value) ((round(value*DECIMAL_RATIO))/DECIMAL_RATIO)

#define HIT_THRESHOLD(input,threshold) ((round(input*DECIMAL_RATIO)) >= (round(threshold*DECIMAL_RATIO)))

#define AS_ZERO_ERR 0.0
#define POSTIVE_ERR 1.0
#define NEGTIVE_ERR -1.0
#define GET_DIRECTION(value) ((abs(value) < MIN_VALUE_EPS) ? AS_ZERO_ERR : ((value>0.0)?POSTIVE_ERR:NEGTIVE_ERR))
	
	auto FuncGetErrorValue = [](nfloat val) {return GET_DIRECTION(val); };
	auto FuncWriteErrorValueStr = [](nfloat err, std::string& str) {
		str += "Error : ";
		str += std::to_string(err);
		str += "\n";
		};
	auto FuncWriteWeights = [](Neuron* neu, std::string& str) {
		if (!neu) { return; }
		auto wei = neu->GetWeightView();
		auto counter{ 1 };
		for (auto& w : wei) {
			auto decWeight = SPEC_DECIMAL_VALUE(w);
			decWeight = fabs(decWeight) < MIN_VALUE_EPS ? 0.0 : decWeight;
			str += "weight ";
			str += std::to_string(counter);
			str += " : ";
			str += std::to_string(decWeight);
			str += "\n";
			counter++;
		}
		};
	auto FuncWriteWeightStrWithInput = [](Neuron* neu, const DataView<nfloat>& _data, std::string& str) {
		if (!neu) { return; }
		auto wei = neu->GetWeightView();
		auto counter{ 1 };
		auto beg = _data.begin();
		auto end = _data.end();
		for (auto& w : wei) {
			if (beg == end) { break; }
			auto counterStr = std::to_string(counter);
			str += "weight ";
			str += counterStr;
			str += " : ";
			str += std::to_string(SPEC_DECIMAL_VALUE(w));

			str += "\t input ";
			str += counterStr;
			str += " : ";
			str += std::to_string(SPEC_DECIMAL_VALUE(*beg));

			str += "\t calc ";
			str += counterStr;
			str += " : ";
			str += std::to_string(SPEC_DECIMAL_VALUE(w) * SPEC_DECIMAL_VALUE(*beg));
			str += "\n";
			beg++; counter++;
		}
		};
#define ACTIVE 1.0
#define INACTIVE 0.0
	static constexpr size_t CountInput = 2;

	NeuronPool pool(-0.5, 0.5);
	pool.FuncGetErrorValue = FuncGetErrorValue;

	pool.FuncWriteWeights = FuncWriteWeights;
	pool.FuncWriteErrorValueStr = FuncWriteErrorValueStr;
	pool.FuncWriteWeightStrWithInput = FuncWriteWeightStrWithInput;

	pool.FuncSetResult = [](nfloat res, nfloat _th) { return HIT_THRESHOLD(res, _th) ? ACTIVE : INACTIVE; };
	pool.LearningRate() = 0.1;
	pool.Threshold() = 0.2;
	pool.AddNeuron(CountInput);

	auto tDatas = GetTrainingDatas<CountInput>();

	auto counter = 0;
	while (1) {
		PrintLine("Epoch ", counter++, " : ");
		pool.RunEpoch(tDatas);
		system("pause");
	}
}