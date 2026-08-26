extends Node2D

@onready var paddle: CharacterBody2D = $Paddle
@onready var ball: CharacterBody2D = $Ball
@onready var target: Node2D = $Target

var score: int = 0
const BALL_RADIUS := 10
var score_printer: ScorePrinter

func _ready() -> void:
	score_printer = ScorePrinter.new()
	score_printer.print_score(score)

	randomize()
	reset_game()


func _physics_process(_delta: float) -> void:
	paddle.set_control(Input.get_axis("ui_left", "ui_right"))

	if ball.global_position.y < 0 || 512 < ball.global_position.y || ball.global_position.x < 0 || 512 < ball.global_position.x:
		change_score(-1)
		return

	# Ball reached the target.
	if ball.global_position.distance_to(ball.target_position) <= BALL_RADIUS * 2.0:
		change_score(1)
		return


func change_score(amount: int) -> void:
	score += amount
	score_printer.print_score(score)

	reset_game()


func reset_game() -> void:
	# Center paddle.
	paddle.global_position = Vector2(256, 512)
	paddle.set_control(0.0)

	# Center ball.
	ball.reset_ball(Vector2(256, 256))

	# Generate a new target.
	ball.target_position = Vector2(randf_range(0, 512), randf_range(0, 512))
	target.global_position = ball.target_position

	print("Target: ", ball.target_position)
