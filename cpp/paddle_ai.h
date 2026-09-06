#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <torch/torch.h>

using namespace godot;

class PaddleAI : public RefCounted {
GDCLASS(PaddleAI, RefCounted);

private:
float reward_baseline = 0.0f;

struct NetworkImpl : torch::nn::Module {
	torch::nn::Linear layer1{nullptr};
	torch::nn::Linear layer2{nullptr};
	torch::nn::Linear layer3{nullptr};
	torch::nn::Linear output{nullptr};

	NetworkImpl();

	torch::Tensor forward(torch::Tensor x);
};

TORCH_MODULE(Network);

Network network;

std::shared_ptr<torch::optim::Adam> optimizer;

// The single decision made for the current target
torch::Tensor last_log_prob;
bool has_decision = false;

static constexpr float FIELD_SIZE = 512.0f;
float step_count = 0;

static constexpr const char *MODEL_PATH = "user://paddle_ai.bin";

void save_model();
void load_model();

protected:
static void _bind_methods();

public:
PaddleAI();

void save();

// Called once per target; returns desired paddle x in world coords (0..512)
double predict(double target_x, double target_y);

// Trains the single decision with the round's reward
void train(float reward);
};
