#pragma once
#include <vector>

#include "glm/glm.hpp"

#include "wavefront.h"

namespace primitives3d
{
	struct UV_VERTEX_INDEX {
		glm::vec2 UV;
		glm::vec3 NORMAL;
		unsigned int INDEX;
	};

	class Cylinder
	{
		friend class Mesh;
	private:
		float m_Radius;
		float m_Height;
		int m_Sides;
		glm::vec3 m_Position;
		std::vector<glm::vec3> m_Vertices;
		std::vector<UV_VERTEX_INDEX> m_UVVertexIndices;
		std::vector<std::vector<unsigned int>>m_Polys;
		float* m_DrawVertices;
		unsigned int* m_DrawIndices;

	public:
		Cylinder(float radius, float height, int sides, glm::vec3 position = glm::vec3(0.0f));
		const float* vertices();
		const unsigned int* indices();
		inline unsigned int vertexCount() const { return m_UVVertexIndices.size(); };
		inline unsigned int indexCount() const {
			size_t num_inds = 0;
			for (auto i : m_Polys)
			{
				num_inds += (i.size() - 2) * 3;
			}
			return num_inds;
		};
		inline  glm::vec3 position() const { return m_Position; };
		~Cylinder();
	};

	class Box
	{
		friend class Mesh;
	private:
		float m_Length;
		float m_Width;
		float m_Height;
		glm::vec3 m_Position;
		std::vector<glm::vec3> m_Vertices;
		std::vector<UV_VERTEX_INDEX> m_UVVertexIndices;
		std::vector<std::vector<unsigned int>>m_Polys;
		float* m_DrawVertices;
		unsigned int* m_DrawIndices;
	public:
		Box(float length, float width, float height, glm::vec3 position = glm::vec3(0.0f));
		const float* vertices();
		const unsigned int* indices();
		inline unsigned int vertexCount() const { return m_UVVertexIndices.size(); };
		inline unsigned int indexCount() const {
			size_t num_inds = 0;
			for (auto i : m_Polys)
			{
				num_inds += (i.size() - 2) * 3;
			}
			return num_inds;
		};
		inline  glm::vec3 position() const { return m_Position; };
		~Box();
	};

	class Plane
	{
		friend class Mesh;
	private:
		float m_Length;
		float m_Width;
		glm::vec3 m_Position;
		std::vector<glm::vec3> m_Vertices;
		std::vector<UV_VERTEX_INDEX> m_UVVertexIndices;
		std::vector<std::vector<unsigned int>>m_Polys;
		float* m_DrawVertices;
		unsigned int* m_DrawIndices;
	public:
		Plane(float length, float width, glm::vec3 position = glm::vec3(0.0f));
		const float* vertices();
		const unsigned int* indices();
		inline unsigned int vertexCount() const { return m_UVVertexIndices.size(); };
		inline unsigned int indexCount() const {
			size_t num_inds = 0;
			for (auto i : m_Polys)
			{
				num_inds += (i.size() - 2) * 3;
			}
			return num_inds;
		};
		inline  glm::vec3 position() const { return m_Position; };
		~Plane();
	};

	class Mesh
	{
	private:
		glm::vec3 m_Position;
		std::vector<glm::vec3> m_Vertices;
		std::vector<UV_VERTEX_INDEX> m_UVVertexIndices;
		std::vector<std::vector<std::vector<unsigned int>>>m_Components;
		std::vector<std::vector<int>> m_adjacencyList;
		float* m_DrawVertices;
		unsigned int* m_DrawIndices;


	public:
		Mesh(const Mesh& mesh, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
		Mesh(const Cylinder& cyl, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
		Mesh(const Box& box, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
		Mesh(const Plane& plane, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
		Mesh(wavefront::Object obj, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));

		//void append(const Mesh &mesh);

		//void translate(std::vector<glm::Vec3>, glm::mat4);
		//void translate(unsigned int vertex_index, glm::mat4 trasnformation matrix);

		//void recalculateNormals();

		const float* vertices();
		const unsigned int* indices();
		inline unsigned int vertexCount() const { return m_UVVertexIndices.size(); };
		inline unsigned int indexCount() const {
			size_t num_inds = 0;
			for (auto i : m_Components)
			{
				for (auto j : i)
				{
					num_inds += (j.size() - 2) * 3;
				}
			}
			return num_inds;
		};
		inline  glm::vec3 position() const { return m_Position; };
		~Mesh();
	};

	const std::vector<unsigned int> key_to_index(std::string key);
}