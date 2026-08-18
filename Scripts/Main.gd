extends Node2D

@onready var paddle: CharacterBody2D = $Paddle

func _physics_process(delta: float) -> void:
	paddle.set_control(Input.get_axis("ui_left", "ui_right"))
