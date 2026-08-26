#include "paddle_ai.h"

#include <godot_cpp/core/class_db.hpp>

void PaddleAI::_bind_methods() {
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

torch::Tensor PaddleAI::predict(const torch::Tensor& input) {
    torch::NoGradGuard no_grad;

    return torch::sigmoid(network->forward(input));
}
