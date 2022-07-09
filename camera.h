#pragma once

#include <vector>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "framebuffer.h"

class Camera
{
private:
	float m_FOV;
	float m_NearPlane;
	float m_FarPlane;
	unsigned int m_Height;
	unsigned int m_Width;
	glm::vec3 m_Position;
	glm::vec3 m_Target;
	glm::vec3 m_Up;
	glm::vec3 m_Right;
	bool m_Locked;
	glm::vec3 m_LockTo;
	std::vector<Framebuffer> m_Framebuffers;
	unsigned int m_ActiveFramebuffer;
	std::vector<glm::vec2> m_Offsets;
	unsigned int m_ActiveOffset;

public:
	Camera(float FOV, float nearPlane, float farPlane, unsigned int height, unsigned int width, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 matrix();
	void farPlane(float far_plane);
	void nearPlane(float near_plane);
	void move(glm::vec3 dir);
	void FOV(float angle);
	void lockTo(glm::vec3 target, bool lookAt = false);
	void unlock();
	void rotate(float angle, glm::vec3 pivot);
	void addFramebuffer();
	void calculateOffsets(unsigned int number_of_samples);
	void calculateOffsets();
	void setActiveFramebuffer(unsigned int fbid);
	void setActiveOffset(unsigned int fbid);

	inline glm::vec3 position() const { return m_Position; }
	inline glm::vec3 target() const { return m_Target; }
	inline glm::vec3 orientation() const { return m_Target - m_Position; }
	inline glm::vec3 up() const { return m_Up; }
	inline glm::vec3 right() const { return m_Right; }
	inline glm::vec3 lockTarget() const { return m_LockTo; }
	inline bool locked() const { return m_Locked; }
	inline unsigned int width() const { return m_Width; }
	inline unsigned int height() const { return m_Height; }
	inline float nearPlane() const { return m_NearPlane; }
	inline float farPlane() const { return m_FarPlane; }
	inline const Framebuffer& framebuffer(unsigned int index) const { return m_Framebuffers[index]; }
	inline const Framebuffer& framebuffer() const { return m_Framebuffers[m_ActiveFramebuffer]; }
	inline const glm::vec2& offset(unsigned int index) const { return m_Offsets[index]; }
	inline const glm::vec2& offset() const { return m_Offsets[m_ActiveOffset]; }
	inline int FBSize() const { return m_Framebuffers.size(); }
	inline int offsetSize() const { return m_Offsets.size(); }
	inline int activeFB() const { return m_ActiveFramebuffer; }
	inline int activeOffset() const { return m_ActiveOffset; }
};

void CameraInputControl(Camera &camera, GLFWwindow* window, float speed, float sensitivity, float d_time);