#include <sstream>
#include <fstream>
#include <iostream>
#include "wavefront.h"
#include <string.h>

wavefront::Object wavefront::parse_obj_file(std::string filename)
{
	wavefront::Object objs;
	
	std::fstream file;
	file.open(filename);
	if (!file.is_open())
	{
		std::cout << "error: wavefront parser failed to open \"" << filename << "\" and returned empty object." << std::endl;
		return objs;
	}

	std::stringstream ss;
	ss << file.rdbuf();
	file.close();
	std::string line;
	while (getline(ss, line))
	{
		if (line[0] == '#' || line.empty())
		{
			continue;
		}
		//std::cout << line << std::endl;
		std::vector<std::string> parsed_line = line_to_words(line, " ");
		if (parsed_line[0] == "g")
		{
			objs.m_SubObjects.push_back(parsed_line[1]);
		}

		if (parsed_line[0] == "v")
		{
			objs.m_Vertices.push_back(glm::vec3(std::stof(parsed_line[1]), std::stof(parsed_line[2]), std::stof(parsed_line[3])));
		}

		if (parsed_line[0] == "vt")
		{
			objs.m_UVs.push_back(glm::vec2(std::stof(parsed_line[1]), std::stof(parsed_line[2])));
		}

		if (parsed_line[0] == "vn")
		{
			objs.m_Normals.push_back(glm::vec3(std::stof(parsed_line[1]), std::stof(parsed_line[2]), std::stof(parsed_line[3])));
		}

		if (parsed_line[0] == "f")
		{
			
			std::vector<glm::ivec3> poly;
			for (int i = 0; i < parsed_line.size() - 1; i++)
			{
				std::stringstream vertexStream;
				std::vector<std::string> indvec = line_to_words(parsed_line[i + 1], "/");
				//std::cout << "indvec size: " << indvec.size() << std::endl;
				//std::cout << "indvec 0: " << indvec[0] << std::endl;
				//std::cout << "indvec 1: " << indvec[1] << std::endl;
				//std::cout << "indvec 2: " << indvec[2] << std::endl;
				int V_ind = std::stoi(indvec[0]);
				int T_ind = std::stoi(indvec[1]);
				int N_ind = std::stoi(indvec[2]);
				poly.push_back(glm::ivec3(V_ind - 1, T_ind - 1, N_ind - 1));
			}
			if (objs.m_SubObjects.empty())
			{
				objs.m_SubObjects.push_back(std::string("defult"));
			}
			objs.m_SubObjects.back().POLYS.push_back(poly);
		}
	}
	return objs;
}

std::vector<std::string> wavefront::line_to_words(std::string str, std::string delim_list)
{
	std::vector<std::string> words;
	char* cstr = new char[str.size()+1];
	for (int i = 0; i < str.size(); i++)
	{
		cstr[i] = str[i];
	}
	cstr[str.size()] = '\0';
	char* pch;
	char* next_token;
	pch = strtok_s(cstr, delim_list.c_str(), &next_token);
	while (pch != NULL)
	{
		words.push_back(pch);
		pch = strtok_s(NULL, delim_list.c_str(), &next_token);
	}
	delete[] cstr;
	return words;
}

void wavefront::Object::print() const
{
	using namespace std;
	cout << fixed;
	for (auto i : m_SubObjects)
	{
		cout << "sub: " << i.NAME << endl;
		int count = 0;
		for (auto j : i.POLYS)
		{
			cout << "	face " << count << ":" << endl;
			count++;
			for (auto k : j)
			{
				cout << "		V: " << m_Vertices[k.x].x << " " << m_Vertices[k.x].y << " " << m_Vertices[k.x].z << endl;
				cout << "		T: " << m_UVs[k.y].x << " " << m_UVs[k.y].y << endl;
				cout << "		N: " << m_Normals[k.z].x << " " << m_Normals[k.z].y << " " << m_Normals[k.z].z << endl;
				cout << endl;
			}
		}
	}
}
