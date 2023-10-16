#include <nknuse/ai/nknuse_ai.hpp>
#include <nknuse/hw/ai_20231002.hpp>
using namespace nknuse::ai::component;
using namespace nknuse::ai;
using namespace nknuse;
using namespace nknuse::hw;

template<class T>
bool IsValidInput(T& _dst, const std::string& inputVal) {
	return TryCastNumber(_dst, inputVal) ? _dst > 0:false;
}

int main() {
	size_t inputCount{};
	nfloat learningRate{};
	nfloat threshold{};

	ArgumentInputLoop("Count of input", IsValidInput<size_t>, inputCount);
	ArgumentInputLoop("Learning rate", IsValidInput<nfloat>, learningRate);
	ArgumentInputLoop("Threshold", IsValidInput<nfloat>, threshold);

	std::string outputDir = "";
	AI_20231002 test{ inputCount, -0.5,0.5 };
	test.LearningRate() = learningRate;
	test.Threshold() = threshold;
	test.Execute(outputDir);
}
