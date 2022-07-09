#include <iostream>
#include "shader.h"
#include "renderutils.h"

Shader::Shader(uint16_t type): m_Type(type), m_ID(glCreateShader(type)), m_Valid(false)
{
}


Shader::Shader(const Shader& s): m_Type(s.m_Type), m_ID(glCreateShader(s.m_Type)), m_Valid(false)
{
	if (s.m_Valid)
	{
		m_Code = s.m_Code;
		const char* cstrCode = m_Code.c_str();
		GLDEBUG(glShaderSource(m_ID, 1, &cstrCode, NULL));
		m_Valid = compileShader();
	}
}

Shader& Shader::operator=(const Shader& s)
{
	glDeleteShader(m_ID);
	m_Type = s.m_Type;
	m_Code = "";
	m_Valid = false;
	m_ID = glCreateShader(m_Type);
	if (s.m_Valid)
	{
		m_Code = s.m_Code;
		const char* cstrCode = m_Code.c_str();
		GLDEBUG(glShaderSource(m_ID, 1, &cstrCode, NULL));
		m_Valid = compileShader();
	}
	return *this;
}

Shader::~Shader()
{
	glDeleteShader(m_ID);
}

bool Shader::compileShader()
{
	glCompileShader(m_ID);
	int result;
	glGetShaderiv(m_ID, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(m_ID, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(m_ID, length, &length, message);
		switch (m_Type)
		{
		case GL_VERTEX_SHADER:
			std::cout << "Failed to compile vertex shader" << std::endl;
			break;
		case GL_GEOMETRY_SHADER:
			std::cout << "Failed to compile geometry shader" << std::endl;
			break;
		case GL_FRAGMENT_SHADER:
			std::cout << "Failed to compile fragment shader" << std::endl;
			break;
		default:
			break;
		}
		std::cout << message << std::endl;
	}
	return result;
}

void Shader::loadSource(std::string filename)
{
	std::string code;
	readfile(filename, code);
	m_Code = code;
	const char * cstrCode = m_Code.c_str();
	GLDEBUG(glShaderSource(m_ID, 1, &cstrCode, NULL));
	m_Valid = compileShader();
}

void Shader::stringSource(std::string code)
{
	m_Code = code;
	const char* cstrCode = m_Code.c_str();
	GLDEBUG(glShaderSource(m_ID, 1, &cstrCode, NULL));
}



ShaderProgram::ShaderProgram(): m_ID(glCreateProgram()), m_Valid(false)
{
}

ShaderProgram::~ShaderProgram()
{
	GLDEBUG(glUseProgram(0));
	GLDEBUG(glDeleteProgram(m_ID));
}

void ShaderProgram::addShader(Shader shader)
{
	GLDEBUG(glAttachShader(m_ID, shader.getID()));
}

void ShaderProgram::link()
{
	int valid;
	GLDEBUG(glLinkProgram(m_ID));
	GLDEBUG(glValidateProgram(m_ID));
	GLDEBUG(glGetProgramiv(m_ID, GL_VALIDATE_STATUS, &valid));
	m_Valid = valid;
	int activ_uniforms;
	int length;
	GLDEBUG(glUseProgram(m_ID));
	GLDEBUG(glGetProgramiv(m_ID, GL_ACTIVE_UNIFORMS, &activ_uniforms));
	GLDEBUG(glGetProgramiv(m_ID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &length));
	char* name = (char*)alloca(length * sizeof(char));
	int count;
	unsigned int type;
	int location;
	for (unsigned int i = 0; i < static_cast<unsigned int>(activ_uniforms); i++)
	{
		GLDEBUG(glGetActiveUniform(m_ID, i, length, nullptr, &count, &type, name));
		location = glGetUniformLocation(m_ID, name);
		m_Uniforms.push_back({ location, i, type, count });
		m_UniformKeys[name] = i;
	}
	GLDEBUG(glUseProgram(0));
}

void ShaderProgram::use() const
{
	GLDEBUG(glUseProgram(m_ID));
	{
		uint8_t slot = 1;
		for (auto& i : m_Textures)
		{
			GLDEBUG(glActiveTexture(GL_TEXTURE0 + slot));
			switch (i.Type)
			{
			case GL_SAMPLER_2D:
				//std::cout << "using non ms " << i.ID << std::endl;
				GLDEBUG(glBindTexture(GL_TEXTURE_2D, i.ID));
				break;
			case GL_SAMPLER_2D_MULTISAMPLE:
				//std::cout << "using ms " << i.ID << std::endl;
				GLDEBUG(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, i.ID));
				break;
			case GL_SAMPLER_CUBE:
				//std::cout << "using cubemap " << i.ID << std::endl;
				GLDEBUG(glBindTexture(GL_TEXTURE_CUBE_MAP, i.ID));
				break;
			default:
				break;
			}
			GLDEBUG(glUniform1i(m_Uniforms[i.INDEX].LOCATION, slot));
			slot++;
		}
	}
}

void ShaderProgram::clean()
{
	GLDEBUG(glUseProgram(0));
	GLDEBUG(glDeleteProgram(m_ID));
	m_ID = glCreateProgram();
	m_UniformKeys = std::unordered_map<std::string, int>() ;
	m_Uniforms = std::vector<ShaderUniform>();
	m_Textures = std::vector<TextureInfo>();
}



void ShaderProgram::show_uniforms()
{
	std::vector<std::string> keys;
	std::vector<int> values;

	std::cout << "uniforms for program " << m_ID << ":" << std::endl;

	for (std::unordered_map<std::string, int>::iterator it = m_UniformKeys.begin(); it != m_UniformKeys.end(); it++)
	{
		std::cout << "	" <<it->first << " is at " << it->second << std::endl;
	}

	std::cout << "end of uniforms for program " << m_ID << std::endl;
}


void ShaderProgram::setUniform(const char* name, float value)
{
	use();
	ShaderUniform uni = { 0, 0, 0, 0 };
	if (m_UniformKeys.find(name) != m_UniformKeys.end())
	{
		uni = m_Uniforms[m_UniformKeys[name]];
		if (uni.LOCATION == -1)
		{
			return;
		}
		if (uni.TYPE == GL_FLOAT)
		{
			GLDEBUG(glUniform1f(uni.LOCATION, value));
		}
		else
		{
			std::string type = GLDecode(uni.TYPE, "gltypeslist.txt");
			std::cout << "uniform " << name << " is of type " << type << " and not " << "GL_FLOAT" << std::dec << std::endl;
		}
	}
	//else
	//{
	//	std::cout << "no uniform \"" << name << "\" found!" << std::endl;
	//}
	return;
}

void ShaderProgram::setUniform(const char* name, const std::vector<float>& values)
{
	use();
	ShaderUniform uni = { 0, 0, 0, 0 };
	if (m_UniformKeys.find(std::string(name)+"[0]") != m_UniformKeys.end())
	{
		uni = m_Uniforms[m_UniformKeys[std::string(name) + "[0]"]];
		if (uni.LOCATION == -1)
		{
			return;
		}
		if (uni.TYPE == GL_FLOAT)
		{
			GLDEBUG(glUniform1fv(uni.LOCATION, values.size(), values.data()));
		}
		else
		{
			std::string type = GLDecode(uni.TYPE, "gltypeslist.txt");
			std::cout << "uniform " << name << " is of type " << type << " and not " << "GL_FLOAT" << std::dec << std::endl;
		}
	}
	//else
	//{
	//	std::cout << "no uniform \"" << name << "\" found!" << std::endl;
	//}
	return;
}

void ShaderProgram::setUniform(const char* name, int value)
{
	use();
	ShaderUniform uni = { 0, 0, 0, 0 };
	if (m_UniformKeys.find(name) != m_UniformKeys.end())
	{
		uni = m_Uniforms[m_UniformKeys[name]];
		if (uni.LOCATION == -1)
		{
			return;
		}
		if (uni.TYPE == GL_INT || uni.TYPE == GL_UNSIGNED_INT)
		{
			GLDEBUG(glUniform1i(uni.LOCATION, value));
		}
		else
		{
			std::string type = GLDecode(uni.TYPE, "gltypeslist.txt");
			std::cout << "uniform " << name << " is of type " << type << " and not " << "GL_UNSIGNED_INT" << " or " << "GL_INT" << std::dec << std::endl;
		}
	}
	//else
	//{
	//	std::cout << "no uniform \"" << name << "\" found!" << std::endl;
	//}
	return;
}

void ShaderProgram::setUniform(const char* name, unsigned int value)
{
	use();
	ShaderUniform uni = { 0, 0, 0, 0 };
	if (m_UniformKeys.find(name) != m_UniformKeys.end())
	{
		uni = m_Uniforms[m_UniformKeys[name]];
		if (uni.LOCATION == -1)
		{
			return;
		}		
		if (uni.TYPE == GL_INT || uni.TYPE == GL_UNSIGNED_INT)
		{
			GLDEBUG(glUniform1i(uni.LOCATION, value));
		}
		else
		{
			std::string type = GLDecode(uni.TYPE, "gltypeslist.txt");
			std::cout << "uniform " << name << " is of type " << type << " and not " << "GL_UNSIGNED_INT" << " or " << "GL_INT" << std::dec << std::endl;
		}
	}
	//else
	//{
	//	std::cout << "no uniform \"" << name << "\" found!" << std::endl;
	//}
	return;
}

void ShaderProgram::setUniform(const char* name, const glm::vec2& value)
{
	use();
	ShaderUniform uni = { 0, 0, 0, 0 };
	if (m_UniformKeys.find(name) != m_UniformKeys.end())
	{
		uni = m_Uniforms[m_UniformKeys[name]];
		if (uni.LOCATION == -1)
		{
			std::cout << "found" << name << " : x= " << value.x << " , y= " << value.y << std::endl;
			return;
		}
		if (uni.TYPE == GL_FLOAT_VEC2)
		{
			GLDEBUG(glUniform2f(uni.LOCATION, value.x, value.y));
		}
		else
		{
			std::string type = GLDecode(uni.TYPE, "gltypeslist.txt");
			std::cout << "uniform " << name << " is of type " << type << " and not " << "GL_FLOAT_VEC2" << std::dec << std::endl;
		}
	}
	//else
	//{
	//	std::cout << "no uniform \"" << name << "\" found!" << std::endl;
	//}
	return;
}

void ShaderProgram::setUniform(const char* name, const glm::ivec2& value)
{
	use();
	ShaderUniform uni = { 0, 0, 0, 0 };
	if (m_UniformKeys.find(name) != m_UniformKeys.end())
	{
		uni = m_Uniforms[m_UniformKeys[name]];
		if (uni.LOCATION == -1)
		{
			return;
		}
		if (uni.TYPE == GL_INT_VEC2)
		{
			GLDEBUG(glUniform2i(uni.LOCATION, value.x, value.y));
		}
		else
		{
			std::string type = GLDecode(uni.TYPE, "gltypeslist.txt");
			std::cout << "uniform " << name << " is of type " << type << " and not " << "GL_INT_VEC2" << std::dec << std::endl;
		}
	}
	//else
	//{
	//	std::cout << "no uniform \"" << name << "\" found!" << std::endl;
	//}
	return;
}

void ShaderProgram::setUniform(const char* name, const glm::vec3& value)
{
	use();
	ShaderUniform uni = { 0, 0, 0, 0 };
	if (m_UniformKeys.find(name) != m_UniformKeys.end())
	{
		uni = m_Uniforms[m_UniformKeys[name]];
		if (uni.LOCATION == -1)
		{
			return;
		}
		if (uni.TYPE == GL_FLOAT_VEC3)
		{
			GLDEBUG(glUniform3f(uni.LOCATION, value.x, value.y, value.z));
		}
		else
		{
			std::string type = GLDecode(uni.TYPE, "gltypeslist.txt");
			std::cout << "uniform " << name << " is of type " << type << " and not " << "GL_FLOAT_VEC3" << std::dec << std::endl;
		}
	}
	//else
	//{
	//	std::cout << "no uniform \"" << name << "\" found!" << std::endl;
	//}
	return;
}

void ShaderProgram::setUniform(const char* name, const glm::vec4& value)
{
	use();
	ShaderUniform uni = { 0, 0, 0, 0 };
	if (m_UniformKeys.find(name) != m_UniformKeys.end())
	{
		uni = m_Uniforms[m_UniformKeys[name]];
		if (uni.LOCATION == -1)
		{
			return;
		}
		if (uni.TYPE == GL_FLOAT_VEC4)
		{
			GLDEBUG(glUniform4f(uni.LOCATION, value.x, value.y, value.z, value.w));
		}
		else
		{
			std::string type = GLDecode(uni.TYPE, "gltypeslist.txt");
			std::cout << "uniform " << name << " is of type " << type << " and not " << "GL_FLOAT_VEC4" << std::dec << std::endl;
		}
	}
	//else
	//{
	//	std::cout << "no uniform \"" << name << "\" found!" << std::endl;
	//}
	return;
}

void ShaderProgram::setUniform(const char* name, const glm::mat3& value)
{
	use();
	ShaderUniform uni = { 0, 0, 0, 0 };
	if (m_UniformKeys.find(name) != m_UniformKeys.end())
	{
		uni = m_Uniforms[m_UniformKeys[name]];
		if (uni.LOCATION == -1)
		{
			return;
		}
		if (uni.TYPE == GL_FLOAT_MAT3)
		{
			GLDEBUG(glUniformMatrix3fv(uni.LOCATION, 1, GL_FALSE, glm::value_ptr(value)));
		}
		else
		{
			std::string type = GLDecode(uni.TYPE, "gltypeslist.txt");
			std::cout << "uniform " << name << " is of type " << type << " and not " << "GL_FLOAT_MAT3" << std::dec << std::endl;
		}
	}
	//else
	//{
	//	std::cout << "no uniform \"" << name << "\" found!" << std::endl;
	//}
	return;
}

void ShaderProgram::setUniform(const char* name, const glm::mat4& value)
{
	use();
	ShaderUniform uni = { 0, 0, 0, 0 };
	if (m_UniformKeys.find(name) != m_UniformKeys.end())
	{
		uni = m_Uniforms[m_UniformKeys[name]];
		if (uni.LOCATION == -1)
		{
			return;
		}
		if (uni.TYPE == GL_FLOAT_MAT4)
		{
			GLDEBUG(glUniformMatrix4fv(uni.LOCATION, 1, GL_FALSE, glm::value_ptr(value)));
		}
		else
		{
			std::string type = GLDecode(uni.TYPE, "gltypeslist.txt");
			std::cout << "uniform " << name << " is of type " << type << " and not " << "GL_FLOAT_MAT4" << std::dec << std::endl;
		}
	}
	//else
	//{
	//	std::cout << "no uniform \"" << name << "\" found!" << std::endl;
	//}
	return;
}

void ShaderProgram::setUniform(const char* name, const Texture& texture)
{
	use();
	ShaderUniform uni = { 0, 0, 0, 0 };
	if (m_UniformKeys.find(name) != m_UniformKeys.end())
	{
		unsigned int index = m_UniformKeys[name];
		uni = m_Uniforms[index];
		if (uni.LOCATION == -1)
		{
			return;
		}
		if (uni.TYPE == GL_SAMPLER_2D)
		{
			for (auto& i : m_Textures)
			{
				if (i.INDEX == index)
				{
					i.ID = texture.ID();
					return;
				}
			}
			m_Textures.push_back({ texture.ID(), index, GL_SAMPLER_2D });
		}
		else
		{
			std::string type = GLDecode(uni.TYPE, "gltypeslist.txt");
			std::cout << "uniform " << name << " is of type " << type << " and not " << "GL_SAMPLER_2D" << std::dec << std::endl;
		}
	}
	//else
	//{
	//	std::cout << "no uniform \"" << name << "\" found!" << std::endl;
	//}
	return;
}

void ShaderProgram::setUniform(const char* name, const MultiSampleTexture& mstexture)
{
	use();
	ShaderUniform uni = { 0, 0, 0, 0 };
	if (m_UniformKeys.find(name) != m_UniformKeys.end())
	{
		unsigned int index = m_UniformKeys[name];
		uni = m_Uniforms[index];
		if (uni.LOCATION == -1)
		{
			std::cout << "uniform " << name << " is  not found" << std::endl;
			return;
		}
		if (uni.TYPE == GL_SAMPLER_2D_MULTISAMPLE)
		{
			for (auto& i : m_Textures)
			{
				if (i.INDEX == index)
				{
					i.ID = mstexture.ID();
					return;
				}
			}
			m_Textures.push_back({ mstexture.ID(), index, GL_SAMPLER_2D_MULTISAMPLE });
		}
		else
		{
			std::string type = GLDecode(uni.TYPE, "gltypeslist.txt");
			std::cout << "uniform " << name << " is of type " << type << " and not " << "GL_SAMPLER_2D_MULTISAMPLE" << std::dec << std::endl;
		}
	}
	//else
	//{
	//	std::cout << "no uniform \"" << name << "\" found!" << std::endl;
	//}
	return;
}

void ShaderProgram::setUniform(const char* name, const Cubemap& cubemap)
{
	use();
	ShaderUniform uni = { 0, 0, 0, 0 };
	if (m_UniformKeys.find(name) != m_UniformKeys.end())
	{
		unsigned int index = m_UniformKeys[name];
		uni = m_Uniforms[index];
		if (uni.LOCATION == -1)
		{
			return;
		}
		if (uni.TYPE == GL_SAMPLER_CUBE)
		{
			for (auto& i : m_Textures)
			{
				if (i.INDEX == index)
				{
					i.ID = cubemap.ID();
					return;
				}
			}
			m_Textures.push_back({ cubemap.ID(), index, GL_SAMPLER_CUBE });
		}
		else
		{
			std::string type = GLDecode(uni.TYPE, "gltypeslist.txt");
			std::cout << "uniform " << name << " is of type " << type << " and not " << "GL_SAMPLER_CUBE" << std::dec << std::endl;
		}
	}
	//else
	//{
	//	std::cout << "no uniform \"" << name << "\" found!" << std::endl;
	//}
	return;
}
