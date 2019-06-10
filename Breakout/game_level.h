#ifndef GAMELEVEL_H
#define GAMELEVEL_H

#include <vector>

#include <glad\glad.h>
#include <glm\glm.hpp>

#include "game_object.h"
#include "sprite_renderer.h"
#include "resource_manager.h"

class GameLevel
{
public:
	// Level state
	std::vector<GameObject> Bricks;

	GameLevel() { }
	// Loads level from file
	void Load(const GLchar *file, GLuint levelWidth, GLuint levelHeight);
	// Render level
	void Draw(SpriteRenderer &renderer);
	// Check if the level is completed
	GLboolean IsCompleted();
private:
	// Initialize level from tile data
	void init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);
};

#endif // !GAMELEVEL_H
