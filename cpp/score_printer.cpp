#include "score_printer.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void ScorePrinter::_bind_methods() {
	ClassDB::bind_method(
		D_METHOD("print_score", "score"),
		&ScorePrinter::print_score
	);
}

void ScorePrinter::print_score(int score) {
	UtilityFunctions::print("C++ Score: ", score);
}
