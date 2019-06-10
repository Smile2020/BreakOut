#include "resource_manager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Instantiate static variables
std::map <std::string, Texture2D>	ResourceManager::Textures;
std::map <std::string, Shader>		ResourceManager::Shaders;

Shader & ResourceManager::LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile,
	const GLchar *gShaderFile, std::string name)
{
	Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
	return Shaders[name];
}

Shader & ResourceManager::GetShader(const std::string name)
{
	return Shaders.at(name);
}

Texture2D & ResourceManager::LoadTexture(const GLchar *filename, GLboolean alpha, std::string name)
{
	Textures[name] = loadTextureFromFile(filename, alpha);
	return Textures[name];
}

Texture2D & ResourceManager::GetTexture(std::string name)
{
	return Textures.at(name);
}

void ResourceManager::Clear()
{
	// delete all shaders
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.ID);
	// delete all textures
	for (auto iter : Textures)
		glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile)
{
	// Retrive the source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	try
	{
		//Open files
		std::ifstream vertexShaderFile(vShaderFile);
		std::ifstream fragmentShaderFile(fShaderFile);

		// Read file's buffer contents into streams
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();

		// close file
		vertexShaderFile.close();
		fragmentShaderFile.close();

		//convert stream to string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		// if geometry is present, also load it
		if (gShaderFile != nullptr)
		{
			std::ifstream geometryShaderFile(gShaderFile);
			std::stringstream gShaderStream;
			gShaderStream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryCode = gShaderStream.str();
		}	
	}
	catch (std::exception e)
	{
		std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
	}
	const GLchar *vShaderCode = vertexCode.c_str();
	const GLchar *fShaderCode = fragmentCode.c_str();
	const GLchar *gShaderCode = geometryCode.c_str();

	// create shadcer object
	Shader shader;
	shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
	return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const GLchar *file, GLboolean alpha)
{
	Texture2D texture;
	// change format if alpha is not null
	if (alpha)
	{
		texture.Internal_Format = GL_RGBA;
		texture.Image_Format = GL_RGBA;
	}
	// load image
	int width, height, channel;
	unsigned char *data = stbi_load(file, &width, &height, &channel, 0);
	if (data)
	{
		texture.Generate(width, height, data);
	}
	else
	{
		std::cout << "Failed to lead texture " << file << std::endl;
	}
	stbi_image_free(data);
	return texture;
}

