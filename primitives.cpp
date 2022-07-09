#include <iostream>
#include <unordered_map>
#include <string>
#include <unordered_set>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "primitives.h"



primitives3d::Cylinder::Cylinder(float radius, float height, int sides, glm::vec3 position /*= glm::vec3(0.0f)*/):
	m_Radius(radius), m_Height(height), m_Sides(sides), m_Position(position), m_DrawIndices(nullptr), m_DrawVertices(nullptr)
{
	constexpr float PI = 3.141592653589793238462643383;
	glm::vec3 vertex(0.0f, 0.0f, -radius);

	//bottom vertices
	for (unsigned int i = 0; i < sides; i++)
	{
		m_Vertices.push_back(vertex + position);
		vertex = glm::rotate(vertex, (PI * 2) / (sides), glm::vec3(0, 1, 0));
	}

	//top vertices
	vertex = glm::vec3(0.0f, height, -radius);
	for (unsigned int i = 0; i < sides; i++)
	{
		m_Vertices.push_back(vertex + position);
		vertex = glm::rotate(vertex, (PI * 2) / (sides), glm::vec3(0, 1, 0));
	}

	//bottom wall uvverts
	for (unsigned int j = 0; j <= sides; j++)
	{
		unsigned int i = j % sides;
		glm::vec2 uv = glm::vec2(static_cast<float>(j) / (sides), 0);
		glm::vec3 normal = glm::normalize(m_Vertices[i] - position);
		m_UVVertexIndices.push_back({ uv, normal, i });
	}

	//top wall uvverts
	for (unsigned int j = 0; j <= sides; j++)
	{
		unsigned int i = (j % sides) + sides;
		glm::vec2 uv = glm::vec2(static_cast<float>(j) / (sides), 1);
		glm::vec3 normal = glm::normalize(m_Vertices[i - sides] - position);
		m_UVVertexIndices.push_back({ uv, normal, i });
	}

	//bottom cap uvverts
	for (unsigned int j = sides; j > 0; j--)
	{
		unsigned int i = j - 1;
		glm::vec3 p  = glm::normalize(m_Vertices[i] - position) * 0.5f + 0.5f;
		glm::vec2 uv(p.x, p.z);
		glm::vec3 normal(0, -1, 0);
		m_UVVertexIndices.push_back({ uv, normal, i });
	}

	//top cap uvverts
	for (unsigned int j = 0; j < sides; j++)
	{
		unsigned int i = j + sides;
		glm::vec3 p = glm::normalize(m_Vertices[j] - position) * 0.5f + 0.5f;
		glm::vec2 uv(p.x, 1 - p.z);
		glm::vec3 normal(0, 1, 0);
		m_UVVertexIndices.push_back({ uv, normal, i });
	}

	//wall polys
	for (unsigned int i = 0; i < sides; i++)
	{
		std::vector<unsigned int>poly;
		unsigned int j = i + 1;
		poly.push_back(i);
		poly.push_back(j);
		poly.push_back(j + sides + 1);
		poly.push_back(i + sides + 1);
		m_Polys.push_back(poly);
	}

	//bottom cap poly
	{
		std::vector<unsigned int>poly;
		for (unsigned int j = 0; j < sides; j++)
		{
			unsigned int i = j + 2 * (sides + 1);
			poly.push_back(i);
		}
		m_Polys.push_back(poly);
	}

	//top cap poly
	{
		std::vector<unsigned int>poly;
		for (unsigned int j = 0; j < sides; j++)
		{
			unsigned int i = j + 2 * (sides + 1) + sides;
			poly.push_back(i);
		}
		m_Polys.push_back(poly);
	}
}

const float* primitives3d::Cylinder::vertices()
{
	if (!m_DrawVertices)
	{
		m_DrawVertices = new float[m_UVVertexIndices.size() * (3 + 3 + 3 + 2)];
		for (int j = 0; j < m_UVVertexIndices.size(); j++)
		{
			int i = j * 11;
			m_DrawVertices[i + 0] = m_Vertices[m_UVVertexIndices[j].INDEX].x;
			m_DrawVertices[i + 1] = m_Vertices[m_UVVertexIndices[j].INDEX].y;
			m_DrawVertices[i + 2] = m_Vertices[m_UVVertexIndices[j].INDEX].z;

			m_DrawVertices[i + 3] = (j / static_cast<float>(m_UVVertexIndices.size() - 1));
			m_DrawVertices[i + 4] = (j / static_cast<float>(m_UVVertexIndices.size() - 1));
			m_DrawVertices[i + 5] = (j / static_cast<float>(m_UVVertexIndices.size() - 1));

			m_DrawVertices[i + 6] = m_UVVertexIndices[j].NORMAL.x;
			m_DrawVertices[i + 7] = m_UVVertexIndices[j].NORMAL.y;
			m_DrawVertices[i + 8] = m_UVVertexIndices[j].NORMAL.z;

			m_DrawVertices[i + 9] = m_UVVertexIndices[j].UV.x;
			m_DrawVertices[i + 10] = m_UVVertexIndices[j].UV.y;
		}
	}
	return m_DrawVertices;
}

const unsigned int* primitives3d::Cylinder::indices()
{
	if (!m_DrawIndices)
	{
		size_t num_inds = 0;
		for (auto i : m_Polys)
		{
			num_inds += (i.size() - 2) * 3;
		}
		m_DrawIndices = new unsigned int[num_inds];
		num_inds = 0;
		for (auto i : m_Polys)
		{
			for (int j = 0; j < i.size() - 2; j++)
			{
				m_DrawIndices[num_inds] = i[0];
				num_inds++;
				m_DrawIndices[num_inds] = i[j + 1];
				num_inds++;
				m_DrawIndices[num_inds] = i[j + 2];
				num_inds++;
			}
		}
	}
	return m_DrawIndices;
}

primitives3d::Cylinder::~Cylinder()
{
	if (m_DrawIndices)
		delete[] m_DrawIndices;
	if (m_DrawVertices)
		delete[] m_DrawVertices;
}




primitives3d::Box::Box(float length, float width, float height, glm::vec3 position /*= glm::vec3(0.0f)*/):
m_Length(length), m_Width(width), m_Height(height), m_Position(position), m_DrawIndices(nullptr), m_DrawVertices(nullptr)
{
	glm::vec3 vert;
	glm::vec3 hv(0.0f, height, 0.0f);
	vert = position;
	vert.x -= width * 0.5f;
	vert.z += length * 0.5f;
	m_Vertices.push_back(vert);
	m_Vertices.push_back(vert + hv);
	vert.x += width;
	m_Vertices.push_back(vert);
	m_Vertices.push_back(vert + hv);
	vert.z -= length;
	m_Vertices.push_back(vert);
	m_Vertices.push_back(vert + hv);
	vert.x -= width;
	m_Vertices.push_back(vert);
	m_Vertices.push_back(vert + hv);

	glm::vec2 uv(0.0f, 0.0f);
	glm::vec3 nrml(0.0f, 0.0f, 1.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 0 });
	uv = glm::vec2(0.0f, 1.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 1 });
	uv = glm::vec2(1.0f, 0.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 2 });
	uv = glm::vec2(1.0f, 1.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 3 });

	uv = glm::vec2(0.0f, 0.0f);
	nrml = glm::vec3(1.0f, 0.0f, 0.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 2 });
	uv = glm::vec2(0.0f, 1.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 3 });
	uv = glm::vec2(1.0f, 0.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 4 });
	uv = glm::vec2(1.0f, 1.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 5 });

	uv = glm::vec2(0.0f, 0.0f);
	nrml = glm::vec3(0.0f, 0.0f, -1.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 4 });
	uv = glm::vec2(0.0f, 1.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 5 });
	uv = glm::vec2(1.0f, 0.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 6 });
	uv = glm::vec2(1.0f, 1.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 7 });

	uv = glm::vec2(0.0f, 0.0f);
	nrml = glm::vec3(-1.0f, 0.0f, 0.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 6 });
	uv = glm::vec2(0.0f, 1.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 7 });
	uv = glm::vec2(1.0f, 0.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 0 });
	uv = glm::vec2(1.0f, 1.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 1 });

	uv = glm::vec2(0.0f, 0.0f);
	nrml = glm::vec3(0.0f, 1.0f, 0.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 1 });
	uv = glm::vec2(0.0f, 1.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 7 });
	uv = glm::vec2(1.0f, 0.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 3 });
	uv = glm::vec2(1.0f, 1.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 5 });

	uv = glm::vec2(0.0f, 0.0f);
	nrml = glm::vec3(0.0f, -1.0f, 0.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 6 });
	uv = glm::vec2(0.0f, 1.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 0 });
	uv = glm::vec2(1.0f, 0.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 4 });
	uv = glm::vec2(1.0f, 1.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 2 });
	for (int j = 0; j < 6; j++)
	{
		std::vector<unsigned int> poly;
		int i = j * 4;
		poly.push_back(0 + i);
		poly.push_back(2 + i);
		poly.push_back(3 + i);
		poly.push_back(1 + i);
		m_Polys.push_back(poly);
	}


}

const float* primitives3d::Box::vertices()
{
	if (!m_DrawVertices)
	{
		m_DrawVertices = new float[m_UVVertexIndices.size() * (3 + 3 + 3 + 2)];
		for (int j = 0; j < m_UVVertexIndices.size(); j++)
		{
			int i = j * 11;
			m_DrawVertices[i + 0] = m_Vertices[m_UVVertexIndices[j].INDEX].x;
			m_DrawVertices[i + 1] = m_Vertices[m_UVVertexIndices[j].INDEX].y;
			m_DrawVertices[i + 2] = m_Vertices[m_UVVertexIndices[j].INDEX].z;

			m_DrawVertices[i + 3] = (j / static_cast<float>(m_UVVertexIndices.size() - 1));
			m_DrawVertices[i + 4] = (j / static_cast<float>(m_UVVertexIndices.size() - 1));
			m_DrawVertices[i + 5] = (j / static_cast<float>(m_UVVertexIndices.size() - 1));

			m_DrawVertices[i + 6] = m_UVVertexIndices[j].NORMAL.x;
			m_DrawVertices[i + 7] = m_UVVertexIndices[j].NORMAL.y;
			m_DrawVertices[i + 8] = m_UVVertexIndices[j].NORMAL.z;

			m_DrawVertices[i + 9] = m_UVVertexIndices[j].UV.x;
			m_DrawVertices[i + 10] = m_UVVertexIndices[j].UV.y;
		}
	}
	return m_DrawVertices;
}

const unsigned int* primitives3d::Box::indices()
{
	if (!m_DrawIndices)
	{
		size_t num_inds = 0;
		for (auto i : m_Polys)
		{
			num_inds += (i.size() - 2) * 3;
		}
		m_DrawIndices = new unsigned int[num_inds];
		num_inds = 0;
		for (auto i : m_Polys)
		{
			for (int j = 0; j < i.size() - 2; j++)
			{
				m_DrawIndices[num_inds] = i[0];
				num_inds++;
				m_DrawIndices[num_inds] = i[j + 1];
				num_inds++;
				m_DrawIndices[num_inds] = i[j + 2];
				num_inds++;
			}
		}
	}
	return m_DrawIndices;
}

primitives3d::Box::~Box()
{
	if (m_DrawIndices)
		delete[] m_DrawIndices;
	if (m_DrawVertices)
		delete[] m_DrawVertices;
}



primitives3d::Plane::Plane(float length, float width, glm::vec3 position /*= glm::vec3(0.0f)*/) :
	m_Length(length), m_Width(width), m_Position(position), m_DrawIndices(nullptr), m_DrawVertices(nullptr)
{
	glm::vec3 vert;
	vert = position;
	vert.x -= width * 0.5f;
	vert.z += length * 0.5f;
	m_Vertices.push_back(vert);
	vert.z -= length;
	m_Vertices.push_back(vert);
	vert.x += width;
	vert.z += length;
	m_Vertices.push_back(vert);
	vert.z -= width;
	m_Vertices.push_back(vert);

	glm::vec2 uv(0.0f, 0.0f);
	glm::vec3 nrml(0.0f, 1.0f, 0.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 0 });
	uv = glm::vec2(0.0f, 1.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 1 });
	uv = glm::vec2(1.0f, 0.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 2 });
	uv = glm::vec2(1.0f, 1.0f);
	m_UVVertexIndices.push_back({ uv, nrml, 3 });
	std::vector<unsigned int> poly;
	poly.push_back(0);
	poly.push_back(2);
	poly.push_back(3);
	poly.push_back(1);
	m_Polys.push_back(poly);
}

const float* primitives3d::Plane::vertices()
{
	if (!m_DrawVertices)
	{
		m_DrawVertices = new float[m_UVVertexIndices.size() * (3 + 3 + 3 + 2)];
		for (int j = 0; j < m_UVVertexIndices.size(); j++)
		{
			int i = j * 11;
			m_DrawVertices[i + 0] = m_Vertices[m_UVVertexIndices[j].INDEX].x;
			m_DrawVertices[i + 1] = m_Vertices[m_UVVertexIndices[j].INDEX].y;
			m_DrawVertices[i + 2] = m_Vertices[m_UVVertexIndices[j].INDEX].z;

			m_DrawVertices[i + 3] = (j / static_cast<float>(m_UVVertexIndices.size() - 1));
			m_DrawVertices[i + 4] = (j / static_cast<float>(m_UVVertexIndices.size() - 1));
			m_DrawVertices[i + 5] = (j / static_cast<float>(m_UVVertexIndices.size() - 1));

			m_DrawVertices[i + 6] = m_UVVertexIndices[j].NORMAL.x;
			m_DrawVertices[i + 7] = m_UVVertexIndices[j].NORMAL.y;
			m_DrawVertices[i + 8] = m_UVVertexIndices[j].NORMAL.z;

			m_DrawVertices[i + 9] = m_UVVertexIndices[j].UV.x;
			m_DrawVertices[i + 10] = m_UVVertexIndices[j].UV.y;
		}
	}
	return m_DrawVertices;
}

const unsigned int* primitives3d::Plane::indices()
{
	if (!m_DrawIndices)
	{
		size_t num_inds = 0;
		for (auto i : m_Polys)
		{
			num_inds += (i.size() - 2) * 3;
		}
		m_DrawIndices = new unsigned int[num_inds];
		num_inds = 0;
		for (auto i : m_Polys)
		{
			for (int j = 0; j < i.size() - 2; j++)
			{
				m_DrawIndices[num_inds] = i[0];
				num_inds++;
				m_DrawIndices[num_inds] = i[j + 1];
				num_inds++;
				m_DrawIndices[num_inds] = i[j + 2];
				num_inds++;
			}
		}
	}
	return m_DrawIndices;
}

primitives3d::Plane::~Plane()
{
	if (m_DrawIndices)
		delete[] m_DrawIndices;
	if (m_DrawVertices)
		delete[] m_DrawVertices;
}





primitives3d::Mesh::Mesh(const Mesh& mesh, glm::vec3 position /*= glm::vec3(0.0f, 0.0f, 0.0f)*/) :
	m_Position(position), m_DrawIndices(nullptr), m_DrawVertices(nullptr)
{
	for (auto i : mesh.m_Vertices)
	{
		m_Vertices.push_back(i);
	}
	for (auto i : mesh.m_UVVertexIndices)
	{
		m_UVVertexIndices.push_back(i);
	}
	for (auto i : mesh.m_Components)
	{
		m_Components.push_back(i);
	}
}

primitives3d::Mesh::Mesh(const Cylinder& cyl, glm::vec3 position /*= glm::vec3(0.0f, 0.0f, 0.0f)*/) :
	m_Position(position), m_DrawIndices(nullptr), m_DrawVertices(nullptr)
{
	for (auto i : cyl.m_Vertices)
	{
		m_Vertices.push_back(i + position);
	}
	for (auto i : cyl.m_UVVertexIndices)
	{
		m_UVVertexIndices.push_back(i);
	}
	m_Components.push_back(cyl.m_Polys);
}

primitives3d::Mesh::Mesh(const Box& box, glm::vec3 position /*= glm::vec3(0.0f, 0.0f,0.0f)*/) :
	m_Position(position), m_DrawIndices(nullptr), m_DrawVertices(nullptr)
{
	for (auto i : box.m_Vertices)
	{
		m_Vertices.push_back(i + position);
	}
	for (auto i : box.m_UVVertexIndices)
	{
		m_UVVertexIndices.push_back(i);
	}
	m_Components.push_back(box.m_Polys);
}

primitives3d::Mesh::Mesh(const Plane& plane, glm::vec3 position /*= glm::vec3(0.0f, 0.0f,0.0f)*/) :
	m_Position(position), m_DrawIndices(nullptr), m_DrawVertices(nullptr)
{
	for (auto i : plane.m_Vertices)
	{
		m_Vertices.push_back(i + position);
	}
	for (auto i : plane.m_UVVertexIndices)
	{
		m_UVVertexIndices.push_back(i);
	}
	m_Components.push_back(plane.m_Polys);
}

primitives3d::Mesh::Mesh(wavefront::Object obj, glm::vec3 position /*= glm::vec3(0.0f, 0.0f, 0.0f)*/) :
	m_Position(position), m_DrawIndices(nullptr), m_DrawVertices(nullptr)
{
	for (auto& i : obj.vertices())
	{
		glm::vec3 vert;
		m_Vertices.push_back(i/*/30.0f*/);
	}
	std::unordered_set<std::string> vertex_set;

	for (auto i : obj.subObjects())
	{
		for (auto j : i.POLYS)
		{
			for (auto k : j)
			{
				std::string key = std::to_string(k.x) + " " + std::to_string(k.y) + " " + std::to_string(k.z);
				vertex_set.insert(key);
			}
		}
	}

	std::unordered_map<std::string, unsigned int> index_map;
	{
		unsigned int ind = 0;
		for (auto i : vertex_set)
		{
			std::vector<unsigned int> inds;
			inds = key_to_index(i);
			UV_VERTEX_INDEX UV;
			UV.INDEX = inds[0];
			UV.UV = obj.UVs()[inds[1]];
			UV.NORMAL = obj.normals()[inds[2]];
			m_UVVertexIndices.push_back(UV);
			index_map[i] = ind;
			ind++;
		}
	}

	{
		std::vector<std::vector<std::vector<unsigned int>>> comps;
		for (auto i : obj.subObjects())
		{
			std::vector<std::vector<unsigned int>> comp;
			for (auto j : i.POLYS)
			{
				std::vector<unsigned int> poly;
				for (auto k : j)
				{
					std::string key = std::to_string(k.x) + " " + std::to_string(k.y) + " " + std::to_string(k.z);
					unsigned int ind = index_map[key];
					poly.push_back(ind);
				}
				comp.push_back(poly);
			}
			m_Components.push_back(comp);
		}
	}
}

const float* primitives3d::Mesh::vertices()
{
	if (!m_DrawVertices)
	{
		m_DrawVertices = new float[m_UVVertexIndices.size() * (3 + 3 + 3 + 2)];
		for (int j = 0; j < m_UVVertexIndices.size(); j++)
		{
			int i = j * 11;
			m_DrawVertices[i + 0] = m_Vertices[m_UVVertexIndices[j].INDEX].x;
			m_DrawVertices[i + 1] = m_Vertices[m_UVVertexIndices[j].INDEX].y;
			m_DrawVertices[i + 2] = m_Vertices[m_UVVertexIndices[j].INDEX].z;

			m_DrawVertices[i + 3] = (j / static_cast<float>(m_UVVertexIndices.size() - 1));
			m_DrawVertices[i + 4] = (j / static_cast<float>(m_UVVertexIndices.size() - 1));
			m_DrawVertices[i + 5] = (j / static_cast<float>(m_UVVertexIndices.size() - 1));

			m_DrawVertices[i + 6] = m_UVVertexIndices[j].NORMAL.x;
			m_DrawVertices[i + 7] = m_UVVertexIndices[j].NORMAL.y;
			m_DrawVertices[i + 8] = m_UVVertexIndices[j].NORMAL.z;

			m_DrawVertices[i + 9] = m_UVVertexIndices[j].UV.x;
			m_DrawVertices[i + 10] = m_UVVertexIndices[j].UV.y;
		}
	}
	return m_DrawVertices;
}

const unsigned int* primitives3d::Mesh::indices()
{
	if (!m_DrawIndices)
	{
		size_t num_inds = 0;
		for (auto i : m_Components)
		{
			for (auto j : i)
			{
				num_inds += (j.size() - 2) * 3;
			}
		}
		m_DrawIndices = new unsigned int[num_inds];
		num_inds = 0;
		for (auto i : m_Components)
		{
			for (auto j : i)
			{
				for (int k = 0; k < j.size() - 2; k++)
				{
					m_DrawIndices[num_inds] = j[0];
					num_inds++;
					m_DrawIndices[num_inds] = j[k + 1];
					num_inds++;
					m_DrawIndices[num_inds] = j[k + 2];
					num_inds++;
				}
			}
		}
	}
	return m_DrawIndices;
}

primitives3d::Mesh::~Mesh()
{
	if (m_DrawIndices)
		delete[] m_DrawIndices;
	if (m_DrawVertices)
		delete[] m_DrawVertices;
}

const std::vector<unsigned int> primitives3d::key_to_index(std::string key)
{
	std::vector<std::string> string_indices = wavefront::line_to_words(key, " ");
	std::vector<unsigned int> indices;
	for (auto i : string_indices)
	{
		indices.push_back(std::stoi(i));
	}
	return indices;
}

