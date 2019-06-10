#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <tuple>

#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_level.h"
#include "ball_object.h"
#include "particle_generator.h"
#include "post_processor.h"
#include "power_up.h"

// Represents the current state of the game
enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

class Game
{
public:
	// Game state
	GameState		State;
	GLboolean		Keys[1024];
	GLuint			Width, Height;
	
	// Game Level
	std::vector<GameLevel> levels;
	GLuint level;

	// Constructor/Destructor
	Game(GLuint width, GLuint height);
	~Game();

	// Initialize game state (load all shaders/textures/levels)
	void Init();

	// GameLoop
	void ProcessInput(GLfloat dt);
	void Update(GLfloat dt);
	void Render();

	// player setting
	const glm::vec2 PLAYER_SIZE = glm::vec2(100, 20);
	const GLfloat PLAYER_VELOCITY = 500.0f;
	GameObject *Player;

	// ball setting
	const glm::vec2 INITIAL_BALL_VELOCITY = glm::vec2(100.0f, -350.0f);
	const GLfloat BALL_RADIUS = 12.5f;
	BallObject *Ball;

    // spriterender
	SpriteRenderer * Renderer;

	// colision func
	GLboolean CheckCollisioin(GameObject &one, GameObject &two);
	Collision CheckCollisioin(BallObject &one, GameObject &two);
	void DoCollisions();

	// Reset
	void ResetLevel();
	void ResetPlayer();

	// particle
	ParticleGenerator *Particles;

	// postprocessor
	PostProcessor *Effects;
	GLfloat ShakeTime = 0.0f;

	// powerup
	std::vector<PowerUp> PowerUps;

	void SpawnPowerUps(GameObject &block);
	void UpdatePowerUps(GLfloat dt);
};

#endif
