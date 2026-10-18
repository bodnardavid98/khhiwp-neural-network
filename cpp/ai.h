#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <torch/torch.h>

using namespace godot;

class AI : public RefCounted {
GDCLASS(AI, RefCounted);

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
std::vector<double> fitnesses;

static constexpr const char *MODEL_PATH = "user://ai.bin";

void save_model();
void load_model();

protected:
static void _bind_methods();

public:
AI();

void save();

// Called once per target; returns desired ball/paddle direction and speed
godot::Vector2 predict(double target_x, double target_y);

// Trains the single decision with the round's reward
void train(float reward);

// Add fitness value to sliding sync
void addfitness(double newfitness);
};
