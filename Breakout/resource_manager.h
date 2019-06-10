#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include <glad/glad.h>

#include "texture.h"
#include "shader.h"


class ResourceManager
{
public:
	// Resource storage
	static std::map<std::string, Shader>	Shaders;
	static std::map<std::string, Texture2D>	Textures;
	// Loads a shader program from files
	static Shader & LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile,
		std::string name);
	// Retrieves a stored shader 
	static Shader & GetShader(std::string name);
	// Loads a texture from file
	static Texture2D & LoadTexture(const GLchar *filename, GLboolean alpha, std::string name);
	// Retrieves a stored texture
	static Texture2D & GetTexture(std::string name);
	// De-allocates all loaded resources
	static void Clear();
private:
	// Private constructor
	ResourceManager() { }
	// Loads a shader from file
	static Shader loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile = nullptr);
	// Loads a texture from file
	static Texture2D loadTextureFromFile(const GLchar *filename, GLboolean alpha);
};

#endif // !RESOURCE_MANAGER_H