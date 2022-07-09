#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "texture.h"
#include "iterator"

struct ShaderUniform
{
	int LOCATION;
	unsigned int ID;
	unsigned int TYPE;
	int COUNT;
};

struct TextureInfo
{
	unsigned int ID;
	unsigned int INDEX;
	int Type;
};

class Shader
{
private:
	uint16_t m_Type;
	std::string m_Code;
	unsigned int m_ID;
	bool m_Valid;
	bool compileShader();
public:
	Shader(uint16_t type);
	Shader(const Shader&);
	Shader& operator=(const Shader&);
	~Shader();
	void loadSource(std::string filename);
	void stringSource(std::string filename);
	inline unsigned int getID() { return m_ID; };
};

class ShaderProgram
{
private:
	unsigned int m_ID;
	bool m_Valid;
	std::unordered_map<std::string, int> m_UniformKeys;
	std::vector<ShaderUniform> m_Uniforms;
	std::vector<TextureInfo> m_Textures;

	inline ShaderProgram(const ShaderProgram& p) { };  //to maximize the speed of creation and destruction of shader program objects they do not keep 
	inline void operator=(const ShaderProgram& p) { }; //a copy of their raw shader objects, and therefore cannot be copied. pass only by reference

public:
	ShaderProgram();
	~ShaderProgram();
	void addShader(Shader shader);
	void link();
	void use() const;
	void clean();

	void show_uniforms();

	//uniform setting !!!!!!!!!!!!!EXTEND FOR OTHER TYPES AS NEEDED!!!!!!!!!!!!!//
	void setUniform(const char* name, float value);								//
	void setUniform(const char* name, const std::vector<float>& values);		//
	void setUniform(const char* name, int value);								//
	void setUniform(const char* name, unsigned int value);						//
	void setUniform(const char* name, const glm::ivec2& value);					//
	void setUniform(const char* name, const glm::vec2& value);					//
	void setUniform(const char* name, const glm::vec3& value);					//
	void setUniform(const char* name, const glm::vec4& value);					//
	void setUniform(const char* name, const glm::mat3& value);					//
	void setUniform(const char* name, const glm::mat4& value);					//
	void setUniform(const char* name, const Texture& texture);					//
	void setUniform(const char* name, const MultiSampleTexture& texture);		//
	void setUniform(const char* name, const Cubemap& cubemap);					//
	//--------------------------------------------------------------------------//

	
	inline unsigned int ID() const { return m_ID; };
};
