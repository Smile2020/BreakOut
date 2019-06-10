#ifndef  POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_renderer.h"
#include "shader.h"

class PostProcessor
{
public:
	// State
	Shader PostProcessingShader;
	Texture2D Texture;
	GLuint Width, Height;

	// Options
	GLboolean Confuse, Chaos, Shake;

	// Constructor
	PostProcessor(Shader shader, GLuint width, GLuint height);

	// Prepares the postprocessor's framebuffer operations before rendering
	void BeginRender();

	// stores all th e rendered data into a texture
	void EndRender();

	// Renders the PostProcessor texture quad
	void Render(GLfloat time);

private:
	// Render sate
	GLuint MSFBO, FBO;
	GLuint RBO;
	GLuint VAO;

	// Initialize quad for rendering postprocessing texture
	void initRenderData();
};

#endif // ! POST_PROCESSOR_H
