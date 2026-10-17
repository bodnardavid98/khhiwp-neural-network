extends CharacterBody2D

@export var speed: float = 512
var control1: float = 0
var control2: float = 0

func set_control(value1: float, value2: float) -> void:
	control1 = value1
	control2 = value2

func _physics_process(_delta: float) -> void:
	velocity.x = control1 * speed
	velocity.y = 0
	rotation = deg_to_rad(control2 * 360)

	move_and_slide()

	# Keep the paddle inside the play area
	global_position.x = clamp(global_position.x, 0, 512)
	global_position.y = 512
