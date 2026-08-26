#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <torch/torch.h>

using namespace godot;

class PaddleAI : public RefCounted {
GDCLASS(PaddleAI, RefCounted);

private:
struct NetworkImpl : torch::nn::Module {
	torch::nn::Linear layer1{nullptr};
	torch::nn::Linear layer2{nullptr};
	torch::nn::Linear output{nullptr};

	NetworkImpl();

	torch::Tensor forward(torch::Tensor x);
};

TORCH_MODULE(Network);

Network network;

static constexpr const char *MODEL_PATH = "user://paddle_ai.bin";

void save_model();
void load_model();

protected:
static void _bind_methods();

public:
PaddleAI();

void save();

Array predict(
	float target_x,
	float target_y,
	float paddle_x,
	float paddle_y
	);
};
