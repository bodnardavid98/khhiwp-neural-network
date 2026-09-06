extends CharacterBody2D

@export var speed: float = 256
@export var paddle_hit: bool = false

var target_position: Vector2


func _ready() -> void:
	reset_ball(global_position)


func reset_ball(newposition: Vector2) -> void:
	global_position = newposition
	paddle_hit = false

	# Start by moving downward
	velocity = Vector2(0, 1).normalized() * speed


func _physics_process(delta: float) -> void:
	var collision := move_and_collide(velocity * delta)

	if collision:
		var collider = collision.get_collider()

		if collider is CharacterBody2D and collider.name == "Paddle":
			bounce_from_paddle(collider)
			paddle_hit = true
		else:
			var normal := collision.get_normal()
			velocity = velocity.bounce(normal)
			velocity = velocity.normalized() * speed


func bounce_from_paddle(paddle: CharacterBody2D) -> void:
	var paddle_half_width := 60.0

	# -1 = left edge of paddle
	#  0 = center
	# +1 = right edge
	var hit_position := (global_position.x - paddle.global_position.x) / paddle_half_width
	hit_position = clamp(hit_position, -1.0, 1.0)

	# Maximum angle away from vertical
	var max_angle := deg_to_rad(60.0)
	var angle := hit_position * max_angle

	# 0 degrees = straight upward
	velocity = Vector2(sin(angle), -cos(angle)) * speed
