#include "ball_object.h"

BallObject::BallObject()
	: GameObject(), Radius(12.5f), Stuck(true) { }

BallObject::BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite)
	: GameObject(pos, glm::vec2(radius * 2, radius * 2), sprite, glm::vec3(1.0f), velocity),
	Radius(radius), Stuck(true) { }

glm::vec2 BallObject::Move(GLfloat dt, GLuint window_width)
{
	if (!this->Stuck)
	{
		// move the ball
		this->Position += this->Velocity * dt;
		// check bounds
		if (this->Position.x <= 0.0f)
		{
			this->Velocity.x *= -1;
			this->Position.x = 0.0f;
		}
		else if (this->Position.x + this->Size.x >= window_width)
		{
			this->Velocity.x *= -1;
			this->Position.x = window_width - this->Size.x;
		}
		if (this->Position.y <= 0.0f)
		{
			this->Velocity.y *= -1;
			this->Position.y = 0.0f;
		}
	}
	return this->Position;
}

// Resets the ball to initial Stuck Position
void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
	this->Position = position;
	this->Velocity = velocity;
	this->Stuck = true;
}