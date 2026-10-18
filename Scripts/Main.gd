extends Node2D

enum GameMode {
	PONG,
	BASKETBALL
}

@export var game_mode: GameMode = GameMode.PONG
@onready var paddle: CharacterBody2D = $Paddle
@onready var ball: CharacterBody2D = $Ball
@onready var target: Node2D = $Target

var scorechange: int = 0
var score: int = 0
var fitness: float = 0
var fitnessabove: int = 0
var BALL_RADIUS := 10
var min_distance: float = INF
var max_distance: float = sqrt(512 * 512 + 512 * 512)
var ai: AI
var start_time: String = Time.get_datetime_string_from_system()
var scores: Array[float] = []
var notraining: bool = false
var training: bool = !notraining
var FIELD_SIZE := 512
var FITNESS_GOAL := 90


func _ready() -> void:
	# Set up everything
	ai = AI.new()
	randomize()

	ball.game_mode = game_mode
	paddle.game_mode = game_mode

	reset_game()

	# Increase speed for faster runs
	Engine.time_scale = 10.0
	Engine.max_physics_steps_per_frame *= 20
	Engine.physics_ticks_per_second *= 20


func _physics_process(_delta: float) -> void:
	# No manual control
	# paddle.set_control(Input.get_axis("ui_left", "ui_right"))

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
	scorechange += 1

	# Keep at most 1000 last scores stored
	scores.append(amount)
	if scores.size() > 1000:
		scores.pop_front()

	# Provide sliding average of scores as fitness to model
	if scores.is_empty():
		fitness = 1
	else:
		var sum := 0.0
		for value in scores:
			sum += value
		fitness = (sum / scores.size() + 1) * 50
	ai.addfitness(fitness)

	# Record above FITNESS_GOAL fitness plays, if training is not disabled,
	# get back into training from verification phase on lost streak
	if FITNESS_GOAL < fitness:
		fitnessabove += 1
	else:
		if !notraining:
			training = true
		fitnessabove = 0

	# Last good run needed, save a last time, enter verification phase
	if training && fitnessabove == 100:
		ai.save()
		fitnessabove = 0
		training = false

	# Verification phase is done, send notification, exit
	if fitnessabove == 1000:
		OS.execute("notify-send", ["-u", "critical", "Training done. Started: " + start_time + " Ended: " + Time.get_datetime_string_from_system()])
		get_tree().quit()

	# Train model
	if training:
		var reward := 1.0 - min_distance / max_distance
		if amount == 1:
			reward += 1.0
		ai.train(reward)

	# Save model every 100 game played, print progress
	if scorechange % 100 == 0:
		print("Fitness: ", fitness, ", Score: ", score)
		if training:
			ai.save()

	# Reset game
	reset_game()

func is_good_target(position: Vector2) -> bool:
	# Target too close
	if (ball.global_position.distance_to(position) < BALL_RADIUS * 2):
		return false

	if game_mode == GameMode.BASKETBALL:
		return Vector2(256, 256).distance_to(position) < 256 || 256 < position.y

	# Target under ball
	if position.x <= ball.global_position.x + BALL_RADIUS * 2 && position.x >= ball.global_position.x - BALL_RADIUS * 2 && position.y > ball.global_position.y:
		return false

	# Under paddle
	var vertical_distance := 492.0 - position.y
	if vertical_distance <= 0.0:
		return false

	# In a position that can be hit
	var max_horizontal_distance := tan(deg_to_rad(60.0)) * vertical_distance
	return abs(position.x - 256.0) <= max_horizontal_distance

func reset_game() -> void:
	# Set paddle and ball position
	if game_mode == GameMode.BASKETBALL:
		paddle.global_position = Vector2(-512, -512)
	else:
		paddle.global_position = Vector2(0, 512)
	ball.reset_ball(Vector2(256, 256))

	# Generate a new target, try not to give free points, nor impossible to hit targets
	var targetposition = Vector2(randf_range(0, 512), randf_range(0, 512))
	while !is_good_target(targetposition):
		targetposition = Vector2(randf_range(0, 512), randf_range(0, 512))
	ball.target_position = targetposition
	target.global_position = targetposition

	# Set control provided by the model
	var result: Vector2 = ai.predict(
		float(ball.target_position.x) / float(FIELD_SIZE),
		float(ball.target_position.y) / float(FIELD_SIZE))

	if game_mode == GameMode.BASKETBALL:
		ball.set_control(result.x, result.y)
	else:
		paddle.set_control(result.x, result.y)

	# Reset distance
	min_distance = INF
