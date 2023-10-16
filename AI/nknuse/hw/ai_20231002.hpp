#pragma once
#include <nknuse/ai/nknuse_ai.hpp>

namespace nknuse {
	namespace hw {
		class AI_20231002 {
			using TrainingData = nknuse::ai::TrainingData;
			using NeuronPool = nknuse::ai::component::NeuronPool;
			using Neuron = nknuse::ai::component::Neuron;
		public:

			size_t CountOfInput{};

			static Vec<TrainingData> GetTrainingDatas(size_t _countInput) {
				size_t totalCount = pow(2, _countInput);
				Vec<TrainingData> retVal{};
				retVal.reserve(totalCount);
				for (auto i = 0; i < totalCount; ++i) {
					TrainingData temp{};
					temp._Inputs.resize(_countInput);
					auto ptr = (nfloat*)temp._Inputs.data();
					for (auto j = 0; j < _countInput; ++j) {
						ptr[j] = (nfloat)((i & ((int)1 << j)) >> j);
					}
					temp.Answer = i == totalCount - 1 ? 1 : 0;
					retVal.push_back(std::move(temp));
				}
				return retVal;
			}

			CONSTANT(nfloat) MIN_VALUE_EPS { 0.05 };
			CONSTANT(nfloat) DECIMAL_RATIO { 10 };
			CONSTANT(nfloat) AS_ZERO_ERR { 0.0 };
			CONSTANT(nfloat) POSTIVE_ERR { 1.0 };
			CONSTANT(nfloat) NEGTIVE_ERR { -1.0 };
			CONSTANT(nfloat) ACTIVE { 1.0 };
			CONSTANT(nfloat) INACTIVE { 0.0 };

			static nfloat DecimalRound(nfloat _val) {
				return round(_val * DECIMAL_RATIO) / DECIMAL_RATIO;
			}
			static bool HitThreshold(nfloat _input, nfloat _thres) {
				return round(_input * DECIMAL_RATIO) >= round(_thres * DECIMAL_RATIO);
			}
			static nfloat GetDirection(nfloat _val) {
				return ((abs(_val) < MIN_VALUE_EPS) ? AS_ZERO_ERR : (_val > 0.0 ? POSTIVE_ERR : NEGTIVE_ERR));
			}

			StdPtr<NeuronPool> Pool;

			nfloat& LearningRate() { return this->Pool->LearningRate(); }
			nfloat& Threshold() { return this->Pool->Threshold(); }

			AI_20231002(size_t _countInput,nfloat _weightRange1, nfloat _weightRange2) : CountOfInput(_countInput) {
				this->Pool = std::make_shared<NeuronPool>(_weightRange1, _weightRange2);
			}

			void Execute(const std::string& _outputDir) {
				auto FuncGetErrorValue = [](nfloat val) {return GetDirection(val); };
				auto FuncWriteErrorValueStr = [](nfloat err, std::string& str) {
					std::string errStr = "Error : ";
					if (err >= 0.0) {
						errStr += "+";
					}
					AppendString(str, errStr, err, "\n");
					};
				auto FuncWriteWeights = [](Neuron* neu, std::string& str) {
					if (!neu) { return; }
					auto wei = neu->GetWeightView();
					auto counter{ 1 };
					for (auto& w : wei) {
						auto decWeight = DecimalRound(w);
						decWeight = fabs(decWeight) < MIN_VALUE_EPS ? 0.0 : decWeight;
						if (decWeight >= 0.0) {
							AppendString(str, "weight ", counter, " : ¡@", decWeight, "\n");
						}
						else {
							AppendString(str, "weight ", counter, " : ", decWeight, "\n");
						}
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
						auto counterStr = std::to_string(counter) + " : ";
						auto valW = DecimalRound(w);
						auto valD = DecimalRound(*beg);
						AppendString(str, "weight ", counterStr, valW);
						AppendString(str, ", input ", counterStr, valD);
						AppendString(str, ", output ", counterStr, valW * valD, "\n");
						beg++; counter++;
					}
					};
				auto FuncSetResult = [](nfloat res, nfloat _th) { return HitThreshold(res, _th) ? ACTIVE : INACTIVE; };

				this->Pool->FuncGetErrorValue = FuncGetErrorValue;
				this->Pool->FuncWriteWeights = FuncWriteWeights;
				this->Pool->FuncWriteErrorValueStr = FuncWriteErrorValueStr;
				this->Pool->FuncWriteWeightStrWithInput = FuncWriteWeightStrWithInput;
				this->Pool->FuncSetResult = FuncSetResult;
				this->Pool->AddNeuron(CountOfInput);
				auto tDatas = GetTrainingDatas(CountOfInput);
				auto counter = 0;

				std::string path = _outputDir + "result_";
				AppendString(path, CountOfInput, "_in_1_out.txt");

				while (1) {
					PrintLine("Epoch ", counter++, " : ");
					std::string record{};
					record += "{\n";
					this->Pool->RunEpoch(tDatas, record);
					record += "\n}\n";
					std::fstream out(path, std::ios::out | std::ios::app);
					out.write(record.c_str(), record.length());
					out.close();
					system("pause");
				}
			}
		};
	}
}