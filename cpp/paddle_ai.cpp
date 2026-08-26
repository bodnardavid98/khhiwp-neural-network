#include "paddle_ai.h"

#include <godot_cpp/classes/file_access.hpp>

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

	ClassDB::bind_method(
		D_METHOD("save"),
		&PaddleAI::save
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
	load_model();
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

void PaddleAI::save() {
	save_model();
}

void PaddleAI::save_model() {
	godot::Ref<godot::FileAccess> file =
		godot::FileAccess::open(MODEL_PATH, godot::FileAccess::WRITE);

	if (file.is_null()) {
		godot::UtilityFunctions::print("Could not open model for writing.");
		return;
	}

	auto save_tensor = [&](const torch::Tensor &tensor) {
			torch::Tensor cpu = tensor.detach().cpu().contiguous();

			int64_t count = cpu.numel();

			file->store_64(count);

			godot::PackedByteArray bytes;
			bytes.resize(count * sizeof(float));

			memcpy(
				bytes.ptrw(),
				cpu.data_ptr<float>(),
				count * sizeof(float)
				);

			file->store_buffer(bytes);
		};

	save_tensor(network->layer1->weight);
	save_tensor(network->layer1->bias);

	save_tensor(network->layer2->weight);
	save_tensor(network->layer2->bias);

	save_tensor(network->output->weight);
	save_tensor(network->output->bias);

	file->close();

	godot::UtilityFunctions::print("Paddle AI model saved.");
}

void PaddleAI::load_model() {
	if (!godot::FileAccess::file_exists(MODEL_PATH)) {
		godot::UtilityFunctions::print(
			"No saved Paddle AI model found. Starting fresh."
			);
		return;
	}

	godot::Ref<godot::FileAccess> file =
		godot::FileAccess::open(MODEL_PATH, godot::FileAccess::READ);

	if (file.is_null()) {
		godot::UtilityFunctions::print(
			"Could not open model for reading."
			);
		return;
	}

	auto load_tensor = [&](torch::Tensor &tensor) {
			int64_t count = file->get_64();

			godot::PackedByteArray bytes =
				file->get_buffer(count * sizeof(float));

			torch::Tensor loaded = torch::from_blob(
				bytes.ptrw(),
				{count},
				torch::TensorOptions().dtype(torch::kFloat32)
				).clone();

			tensor = loaded;
		};

	torch::Tensor tensor;

	load_tensor(tensor);
	network->layer1->weight.detach().copy_(
		tensor.reshape(network->layer1->weight.sizes())
		);

	load_tensor(tensor);
	network->layer1->bias.detach().copy_(
		tensor.reshape(network->layer1->bias.sizes())
		);

	load_tensor(tensor);
	network->layer2->weight.detach().copy_(
		tensor.reshape(network->layer2->weight.sizes())
		);

	load_tensor(tensor);
	network->layer2->bias.detach().copy_(
		tensor.reshape(network->layer2->bias.sizes())
		);

	load_tensor(tensor);
	network->output->weight.detach().copy_(
		tensor.reshape(network->output->weight.sizes())
		);

	load_tensor(tensor);
	network->output->bias.detach().copy_(
		tensor.reshape(network->output->bias.sizes())
		);

	file->close();

	godot::UtilityFunctions::print("Paddle AI model loaded.");
}
