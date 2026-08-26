#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

#include "score_printer.h"
#include "paddle_ai.h"

using namespace godot;

void initialize_neuralnetwork_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	GDREGISTER_CLASS(ScorePrinter);
	GDREGISTER_CLASS(PaddleAI);
}

void uninitialize_neuralnetwork_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {

GDExtensionBool GDE_EXPORT neuralnetwork_library_init(
	GDExtensionInterfaceGetProcAddress p_get_proc_address,
	GDExtensionClassLibraryPtr p_library,
	GDExtensionInitialization *r_initialization
) {
	GDExtensionBinding::InitObject init_obj(
		p_get_proc_address,
		p_library,
		r_initialization
	);

	init_obj.register_initializer(initialize_neuralnetwork_module);
	init_obj.register_terminator(uninitialize_neuralnetwork_module);
	init_obj.set_minimum_library_initialization_level(
		MODULE_INITIALIZATION_LEVEL_SCENE
	);

	return init_obj.init();
}

}
