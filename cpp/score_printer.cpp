#include "score_printer.h"
#include <torch/torch.h>
#include <sstream>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void ScorePrinter::_bind_methods() {
	ClassDB::bind_method(
		D_METHOD("print_score", "score"),
		&ScorePrinter::print_score
	);
}

void ScorePrinter::print_score(int score) {
	torch::Tensor x = torch::tensor({static_cast<float>(score)});

	std::ostringstream oss;
	oss << x;

	UtilityFunctions::print("Tensor: ", oss.str().c_str());
	UtilityFunctions::print("C++ Score: ", score);
}
