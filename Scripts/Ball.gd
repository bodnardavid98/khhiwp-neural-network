extends CharacterBody2D

enum GameMode {
	PONG,
	BASKETBALL
}

@export var game_mode: GameMode = GameMode.PONG
@export var speed: float = 256.0
@export var gravity: float = 512.0

var target_position: Vector2

func set_control(value1: float, value2: float) -> void:
	var launch_speed: float = value1 * 512
	var launch_angle: float = deg_to_rad(value2 * 360)
	velocity = Vector2.RIGHT.rotated(launch_angle) * launch_speed


func _ready() -> void:
	reset_ball(global_position)


func reset_ball(newposition: Vector2) -> void:
	global_position = newposition

	# Start by moving downward in Pong mode
	if game_mode == GameMode.PONG:
		velocity = Vector2(0, 1).normalized() * speed


func _physics_process(delta: float) -> void:
	# Gravity: accelerate downward
	if game_mode == GameMode.BASKETBALL:
		velocity.y += gravity * delta

	var collision := move_and_collide(velocity * delta)

	if collision:
		var collider = collision.get_collider()
		var normal := collision.get_normal()
		velocity = velocity.bounce(normal)

		# Preserve constant speed in Pong mode only
		if game_mode == GameMode.PONG:
			velocity = velocity.normalized() * speed
