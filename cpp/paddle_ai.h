#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <torch/torch.h>

using namespace godot;

class PaddleAI : public RefCounted {
	GDCLASS(PaddleAI, RefCounted);

protected:
	static void _bind_methods();

public:
	PaddleAI();

	// 4 inputs:
	// [target_x, target_y, paddle_x, paddle_y]
	//
	// 2 outputs:
	// [left, right]
	torch::Tensor predict(const torch::Tensor& input);

private:
	// Neural network itself.
	struct NetworkImpl : torch::nn::Module {
		NetworkImpl();

		torch::Tensor forward(torch::Tensor x);

		torch::nn::Linear layer1{nullptr};
		torch::nn::Linear layer2{nullptr};
		torch::nn::Linear output{nullptr};
	};

	TORCH_MODULE(Network);

	Network network;
};
