#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>

#include "renderutils.h"
#include "glm/glm.hpp"


unsigned int GLParseError(const std::string& hexstring)
{
	unsigned int ind = hexstring.find("0x");
	if (ind == hexstring.npos)
	{
		return -1;
	}
	std::stringstream ss(hexstring.substr(ind, hexstring.size() - ind));
	unsigned int result;
	ss >> std::hex >> result >> std::dec;
	return result;
}

bool readfile(const std::string& filename, std::string& content)
{
	std::fstream file;
	file.open(filename, file.in);
	if (file.is_open())
	{
		std::stringstream ss;
		ss << file.rdbuf();
		content = ss.str();
		file.close();
		return true;
	}
	std::cout << "failed to open " << filename << std::endl;
	return false;
}

void GLClearError()
{
	while (glGetError());
}

bool GLReport(bool infile, const char* funcname, unsigned int linenum, const char* filename)
{
	GLenum report;
	bool NoError = true;
	if (!infile)
	{
		while (report = glGetError())
		{
			NoError = false;
			std::cout << "[GL ERROR!] (0x" << std::hex << report << " in " << funcname << " on line " << std::dec << linenum << ")" << std::endl;
		}
		return NoError;
	}
	std::string errors;
	readfile("errlist.txt", errors);
	std::string err;
	std::stringstream ss(errors);
	while (report = glGetError())
	{
		NoError = false;
		while (getline(ss, err))
		{
			if (GLParseError(err) == report)
			{
				std::cout << "IN FILE: [ " << filename << " ]" << std::endl;
				std::cout << "[GL ERROR!] (" << err << " in " << funcname << " on line " << std::dec << linenum << ")" << std::endl;;
				break;;
			}
		}
		ss.clear();
		ss.seekg(0);
	}
	return NoError;
}

std::string GLDecode(int code, std::string filename)
{
	bool NoError = true;
	std::string codes;
	readfile(filename, codes);
	std::string codestring;
	std::stringstream ss(codes);
	while (getline(ss, codestring))
	{
		if (GLParseError(codestring) == code)
		{
			return codestring;
		}
	}
	ss.clear();
	ss.seekg(0);
	return "FAILED TO PARSE " + std::to_string(code);
}

std::vector<float> gaussian(int count, float roughness, bool normalize/*=false*/)
{
	constexpr double PI = 3.1415926535897932384626433832;
	std::vector<float> kernel;
	if (count == 1)
	{
		kernel.push_back(1.0f);
		return kernel;
	}

	if (roughness < 0.00001)
	{
		for (int i = 0; i < count; i++)
		{
			kernel.push_back(0);
		}
		kernel[0] = 1;
		return kernel;
	}

	roughness = glm::clamp(roughness, 0.0f, 1.0f);
	roughness *= roughness * 7;

	float sum = 0;

	for (int i = 0; i < count; i++)
	{
		float D;
		glm::vec2 uv(float(i) * 10 / count, roughness);
		float d = uv.y;
		float d2 = d * d;
		float c = 1.0f / (glm::sqrt(2 * PI) * d) ;
		D = c * glm::exp(-(uv.x) * (uv.x) /(2*d2));
		kernel.push_back(D);
		sum += 2 * D;
	}

	sum -= kernel[0];

	if (normalize)
	{
		for (int i = 0; i < count; i++)
		{
			kernel[i] /= sum;
		}
	}

	return kernel;
}


std::vector<float> ggx(int count, float roughness, bool normalize/*=false*/)
{
	constexpr double PI = 3.1415926535897932384626433832;
	std::vector<float> kernel;
	if (count == 1)
	{
		kernel.push_back(1.0f);
		return kernel;
	}

	if (roughness < 0.00001)
	{
		for (int i = 0; i < count; i++)
		{
			kernel.push_back(0);
		}
		kernel[0] = 1;
		return kernel;
	}

	roughness = glm::clamp(roughness, 0.0f, 1.0f);

	float sum = 0;

	for (int i = 0; i < count; i++)
	{
		float D;
		glm::vec2 uv(float(i)/count, roughness);
		float a2 = uv.y * uv.y;
		a2 *= a2;
		float c2;
		c2 = glm::cos((uv.x) * PI / 2);
		c2 = (c2 * c2);
		D = a2 / (PI * (c2 * (a2 - 1.0) + 1.0) * (c2 * (a2 - 1.0) + 1.0));
		kernel.push_back(D);
		sum += 2 * D;
	}
	
	sum -= kernel[0];

	if (normalize)
	{
		for (int i = 0; i < count; i++)
		{
			kernel[i] /= sum;
		}
	}

	return kernel;
}