#pragma once
#include "vertexarray.h"
#include "indexbuffer.h"
#include "shader.h"
#include "camera.h"
#include "primitives.h"

class Renderer
{
private:
	unsigned int m_viewportWidth;
	unsigned int m_viewportHeight;
	glm::vec4 m_ClearColor;
	VertexArray m_VAO;
	VertexBuffer m_VBO;
	IndexBuffer m_EBO;
	primitives3d::Plane m_Plane;
public:
	inline Renderer() : m_viewportWidth(0), m_viewportHeight(0), m_ClearColor(glm::vec4(0.10f, 0.16f, 0.19f, 1.0f)), m_Plane(2.0f, 2.0f, glm::vec3(0.0f, 0.0f, 0.0f))
	{
		m_VBO.data<GLfloat>(m_Plane.vertexCount() * (11), m_Plane.vertices(), GL_STATIC_DRAW);
		m_VBO.addAttribute(LayoutAttribute(0, GL_FLOAT, 3));
		m_VBO.addAttribute(LayoutAttribute(1, GL_FLOAT, 3));
		m_VBO.addAttribute(LayoutAttribute(2, GL_FLOAT, 3));
		m_VBO.addAttribute(LayoutAttribute(3, GL_FLOAT, 2));

		m_EBO.data<GLuint>(m_Plane.indexCount(), m_Plane.indices(), GL_STATIC_DRAW);

		m_VAO.linkVertexBuffer(m_VBO);
		m_VAO.enableAttribute(0);
		m_VAO.enableAttribute(1);
		m_VAO.enableAttribute(2);
		m_VAO.enableAttribute(3);
		m_VAO.linkIndexBuffer(m_EBO);
	};
	void setViewport(unsigned int width, unsigned int height);
	void clear() const;
	void clear(const FramebufferMS& framebuffer) const;
	void clear(const Framebuffer& framebuffer) const;
	void clearColor(glm::vec4);
	void clearColor(glm::vec3);
	void clearDepth() const;
	void clearDepth(const FramebufferMS& framebuffer) const;
	void clearDepth(const Framebuffer& framebuffer) const;
	void drawTris(VertexArray& VAO, IndexBuffer& EBO, ShaderProgram& program, Camera& camera) const;
	void drawTris(VertexArray& VAO, IndexBuffer& EBO, ShaderProgram& program, Camera& camera, std::vector<unsigned int> framebuffers) const;
	void drawLines(VertexArray& VAO, IndexBuffer& EBO, ShaderProgram& program, Camera& camera) const;
	void drawLines(VertexArray& VAO, IndexBuffer& EBO, ShaderProgram& program, Camera& camera, std::vector<unsigned int> framebuffers) const;
	void drawShader(ShaderProgram& program);
	void drawShader(ShaderProgram& program, const Framebuffer& framebuffer);
	Texture blurX(const Texture& tex, float roughness, int width, int height);
	Texture blurX(const Texture& tex, float roughness);
	Texture blurY(const Texture& tex, float roughness, int width, int height);
	Texture blurY(const Texture& tex, float roughness);
	Texture reduceEnv(const Texture& env, int width, int height, int blur);
	Texture reduceEnvGGX(const Texture& env, int width, int height, float roughness, int index = 0);
	CUBEMAP_INITIALIZER renderCubemap(const Texture& env, int dimension, bool srgb = false);
	void saveImage(std::string filepath, const Framebuffer& framebuffer);
	void saveImage(std::string filepath, const Texture& texture);
	void saveImage(std::string filepath);
};

