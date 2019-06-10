#include "particle_generator.h"

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, GLuint amount)
	: shader(shader), texture(texture), amount(amount)
{
	this->init();
}


void ParticleGenerator::init()
{
	GLuint VBO;
	GLfloat particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	// Create default particle instances
	for (GLuint i = 0; i < this->amount; ++i)
		particles.push_back(Particle());
}

// Render all particles
void ParticleGenerator::Draw()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	this->shader.Use();

	for (Particle particle : this->particles)
	{
		if (particle.Life > 0.0f)
		{
			this->shader.SetVector2f("offset", particle.Position);
			this->shader.SetVector4f("color", particle.Color);
			this->texture.Bind();
			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::Update(GLfloat dt, GameObject &object, GLuint newParticles, glm::vec2 offset)
{
	// Add new particles
	for (GLuint i = 0; i < newParticles; ++i)
	{
		int unusedParticle = firstUnusedParticle();
		respawnParticle(particles[unusedParticle], object, offset);
	}

	// Update all particles
	for (GLuint i = 0; i < this->amount; ++i)
	{
		Particle &p = particles[i];
		p.Life -= dt;
		if (p.Life > 0.0f)
		{
			p.Position -= p.Velocity * dt;
			p.Color.a -= dt * 2.5f;
		}
	}
}

GLuint lastUsedParticle = 0;
GLuint ParticleGenerator::firstUnusedParticle()
{
	// search from last used particle
	for (GLuint i = lastUsedParticle; i < this->amount; ++i)
	{
		if (particles[i].Life <= 0.0f) 
		{
			lastUsedParticle = i;
			return i;
		}
	}

	// otherwise, search all
	for (GLuint i = 0; i < lastUsedParticle; ++i)
	{
		if (particles[i].Life <= 0.0f)
		{
			if (particles[i].Life <= 0.0f)
			{
				lastUsedParticle = i;
				return i;
			}
		}
	}

	// override first particle if all alive
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset)
{
	GLfloat random = ((rand() % 100) - 50) / 10.0f;
	GLfloat rColor = 0.5 + ((rand() % 100) / 100.f);
	particle.Position = object.Position + random + offset;
	particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.Life = 1.0f;
	particle.Velocity = object.Velocity * 0.1f;
}

