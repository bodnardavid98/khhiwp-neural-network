extends CharacterBody2D

@export var speed: float = 400.0

var control: float = 0.0


func set_control(value: float) -> void:
	control = clamp(value, -1.0, 1.0)

func _physics_process(delta: float) -> void:
	velocity.x = control * speed
	velocity.y = 0.0

	move_and_slide()

	# Keep the paddle inside the play area.
	global_position.x = clamp(global_position.x, 60.0, 740.0)
