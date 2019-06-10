#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

// class for texture2d management
class Texture2D
{
public:
	// Holds the ID of the texture object
	GLuint ID;
	// Texture image dimentions in pixels
	GLuint Width, Height;
	// Texture Format
	GLuint Internal_Format;
	GLuint Image_Format;
	// Texture configuration
	GLuint Wrap_S; // Wrapping mode on S axis
	GLuint Wrap_T; // Wrapping mode on T axis
	GLuint Filter_Min; // Filtering mode if texture pixels < screen pixels
	GLuint Filter_Mag; // Filtering mode if texture pixels > screen pixels
	// Constructor (sets default texture modes)
	Texture2D();
	// Generates texture from image data
	void Generate(GLuint width, GLuint height, unsigned char *data);
	// Binds the texture as the current active GL_TEXTURE_2D texture object
	void Bind() const;
};

#endif