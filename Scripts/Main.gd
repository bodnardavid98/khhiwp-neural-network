extends Node2D

@onready var paddle: CharacterBody2D = $Paddle
@onready var ball: CharacterBody2D = $Ball
@onready var target: Node2D = $Target

var scorechange: int = 0
var score: int = 0
var hundredscore: float = 0
var fitness: float = 0
var BALL_RADIUS := 15
var min_distance: float = INF
var max_distance: float = sqrt(512 * 512 + 512 * 512)
var paddle_ai: PaddleAI


func _ready() -> void:
	# Set up everything
	paddle_ai = PaddleAI.new()
	randomize()
	reset_game()

	# Increase speed for faster runs
	Engine.time_scale = 10.0


func _physics_process(_delta: float) -> void:
	# No manual control
	# paddle.set_control(Input.get_axis("ui_left", "ui_right"))

	# No need to move paddle after hit
	if ball.paddle_hit:
		paddle.set_control(0.0)

	# Track how close ball got to target
	var distance := ball.global_position.distance_to(ball.target_position)
	if distance < min_distance:
		min_distance = distance

	# Game lost
	if ball.global_position.y < 0 || 512 < ball.global_position.y || ball.global_position.x < 0 || 512 < ball.global_position.x:
		change_score(-1)
		return

	# Game won
	if distance <= BALL_RADIUS * 2.0:
		change_score(1)
		return


func change_score(amount: int) -> void:
	score += amount
	hundredscore += amount
	var scale := BALL_RADIUS / 10.0
	$Ball/CollisionShape2D/Polygon2D.scale = Vector2(scale, scale)
	$Target/Polygon2D.scale = Vector2(scale, scale)

	# Train model
	var reward := 1.0 - min_distance / max_distance
	if amount == 1:
		reward += 1.0
	paddle_ai.train(reward)

	# Save model every 300 game played, update fitness
	scorechange += 1
	if scorechange % 300 == 0:
		fitness = (hundredscore / 300 + 1) * 50
		hundredscore = 0
		paddle_ai.save()

	# Reset game
	reset_game()

func is_good_target(position: Vector2) -> bool:
	# No free points (target too close or under ball)
	if (ball.global_position.distance_to(position) < BALL_RADIUS * 2) || (position.x <= ball.global_position.x + BALL_RADIUS * 2 && position.x >= ball.global_position.x - BALL_RADIUS * 2 && position.y > ball.global_position.y):
		return false

	# Under paddle
	var vertical_distance := 492.0 - position.y
	if vertical_distance <= 0.0:
		return false

	# In a position that can be hit
	var max_horizontal_distance := tan(deg_to_rad(60.0)) * vertical_distance
	return abs(position.x - 256.0) <= max_horizontal_distance

func reset_game() -> void:
	# Center paddle and ball
	paddle.global_position = Vector2(256, 512)
	ball.reset_ball(Vector2(256, 256))

	# Generate a new target, try not to give free points, nor impossible to hit targets
	var targetposition = Vector2(randf_range(0, 512), randf_range(0, 512))
	while !is_good_target(targetposition):
		targetposition = Vector2(randf_range(0, 512), randf_range(0, 512))
	ball.target_position = targetposition
	target.global_position = targetposition

	# Set control provided by the model
	paddle.set_control(paddle_ai.predict(
		ball.target_position.x,
		ball.target_position.y
	))

	# Reset distance
	min_distance = INF

	# Print progress
	print("Fitness: ", fitness, ", Score: ", score, ", Target: ", ball.target_position, ", Radius: ", BALL_RADIUS)
