#ifndef POWER_UP_H
#define POWER_UP_H

#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

#include "game_object.h"
#include "texture.h"

const glm::vec2 SIZE_POWERUP = glm::vec2(60, 20);
const glm::vec2 VELOCITY_POWERUP = glm::vec2(0.0f, 150.0f);

class PowerUp : public GameObject
{
	// PowerUp State
	std::string Type;
	GLfloat		Duration;
	GLboolean	Activated;

	// Constructor
	PowerUp(std::string type, glm::vec3 color, GLfloat duration, glm::vec2 position, Texture2D texture)
		: GameObject(position, SIZE_POWERUP, texture, color, VELOCITY_POWERUP), Type(type), Duration(duration), Activated()
	{ }
};

#endif