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
			paddle_hit = true
			
		var normal := collision.get_normal()
		velocity = velocity.bounce(normal)
		velocity = velocity.normalized() * speed
