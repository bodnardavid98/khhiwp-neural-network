#ifndef SCORE_PRINTER_H
#define SCORE_PRINTER_H

#include <godot_cpp/classes/ref_counted.hpp>

using namespace godot;

class ScorePrinter : public RefCounted {
	GDCLASS(ScorePrinter, RefCounted);

protected:
	static void _bind_methods();

public:
	void print_score(int score);
};

#endif
