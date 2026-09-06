#include "paddle_ai.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void PaddleAI::_bind_methods() {
	ClassDB::bind_method(D_METHOD("predict", "target_x", "target_y"),
		&PaddleAI::predict);
	ClassDB::bind_method(D_METHOD("train", "reward"), &PaddleAI::train);
	ClassDB::bind_method(D_METHOD("save"), &PaddleAI::save);
}

PaddleAI::NetworkImpl::NetworkImpl()
	: layer1(2, 32),
	layer2(32, 32),
	layer3(32, 32),
	output(32, 1) {
	register_module("layer1", layer1);
	register_module("layer2", layer2);
	register_module("layer3", layer3);
	register_module("output", output);
}

torch::Tensor PaddleAI::NetworkImpl::forward(torch::Tensor x) {
	x = torch::relu(layer1->forward(x));
	x = torch::relu(layer2->forward(x));
	x = torch::relu(layer3->forward(x));
	x = output->forward(x);

	return x;
}

PaddleAI::PaddleAI()
	: network(Network()) {
	load_model();

	optimizer = std::make_shared<torch::optim::Adam>(network->parameters());
}

double PaddleAI::predict(double target_x, double target_y) {
	torch::Tensor input = torch::tensor({
		target_x / FIELD_SIZE,
		target_y / FIELD_SIZE
	}).reshape({1, 2});

	torch::Tensor mean = torch::sigmoid(network->forward(input));

	double std = 0.005f + 0.045f * expf(-step_count / 5000.0f);

	torch::Tensor sample = (mean + torch::randn_like(mean) * std).detach();

	torch::Tensor diff = (sample - mean) / std;
	torch::Tensor log_prob = (-0.5f * diff * diff).sum();

	last_log_prob = log_prob;
	has_decision = true;

	return (sample[0][0].item<double>() - 0.5) * 2;
}

void PaddleAI::train(float reward) {
	if (!has_decision) {
		return;
	}

	reward_baseline = 0.95f * reward_baseline + 0.05f * reward;
	float advantage = reward - reward_baseline;
	torch::Tensor loss = -advantage * last_log_prob;

	optimizer->zero_grad();
	loss.backward();
	optimizer->step();

	has_decision = false;
	step_count += 1;
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

	save_tensor(network->layer3->weight);
	save_tensor(network->layer3->bias);

	save_tensor(network->output->weight);
	save_tensor(network->output->bias);

	file->close();

	godot::UtilityFunctions::print("Paddle AI model saved.");
}

void PaddleAI::load_model() {
	if (!godot::FileAccess::file_exists(MODEL_PATH)) {
		godot::UtilityFunctions::print(
			"No saved Paddle AI model found. Starting fresh.");
		return;
	}

	godot::Ref<godot::FileAccess> file =
		godot::FileAccess::open(MODEL_PATH, godot::FileAccess::READ);

	if (file.is_null()) {
		godot::UtilityFunctions::print("Could not open model for reading.");
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
	network->layer3->weight.detach().copy_(
		tensor.reshape(network->layer3->weight.sizes())
		);

	load_tensor(tensor);
	network->layer3->bias.detach().copy_(
		tensor.reshape(network->layer3->bias.sizes())
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
