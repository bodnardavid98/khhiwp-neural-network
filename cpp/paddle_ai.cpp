#include "paddle_ai.h"

#include <godot_cpp/core/class_db.hpp>

void PaddleAI::_bind_methods() {
	ClassDB::bind_method(
		D_METHOD(
			"predict",
			"target_x",
			"target_y",
			"paddle_x",
			"paddle_y"
			),
		&PaddleAI::predict
		);
}

PaddleAI::NetworkImpl::NetworkImpl()
	: layer1(4, 32),
	layer2(32, 32),
	output(32, 2) {
	register_module("layer1", layer1);
	register_module("layer2", layer2);
	register_module("output", output);
}

torch::Tensor PaddleAI::NetworkImpl::forward(torch::Tensor x) {
	x = torch::relu(layer1->forward(x));
	x = torch::relu(layer2->forward(x));
	x = output->forward(x);

	return x;
}

PaddleAI::PaddleAI()
	: network(Network()) {
}

godot::Array PaddleAI::predict(float target_x, float target_y, float paddle_x,
	float paddle_y) {
	torch::NoGradGuard no_grad;

	torch::Tensor input = torch::tensor({
		target_x,
		target_y,
		paddle_x,
		paddle_y
	}).reshape({1, 4});

	torch::Tensor output = torch::sigmoid(
		network->forward(input)
		);

	// For now, threshold the two independent outputs.
	bool press_left = output[0][0].item<float>() >= 0.5f;
	bool press_right = output[0][1].item<float>() >= 0.5f;

	godot::Array result;
	result.append(press_left);
	result.append(press_right);

	return result;
}
