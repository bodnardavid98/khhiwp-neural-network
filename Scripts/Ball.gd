extends CharacterBody2D

@export var speed: float = 300.0

var direction := Vector2(0.7, 1.0).normalized()

func _ready() -> void:
	velocity = direction * speed

func _physics_process(delta: float) -> void:
	var collision := move_and_collide(velocity * delta)

	if collision:
		var normal := collision.get_normal()

		velocity = velocity.bounce(normal)

		# Keep the speed constant after bouncing.
		velocity = velocity.normalized() * speed
