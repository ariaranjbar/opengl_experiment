#include <iostream>
#include <chrono>
#include <string>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "camera.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/vector_angle.hpp"

constexpr float PI = 3.141592653589793238463;

Camera::Camera(float FOV_agle, float nearPlane, float farPlane, unsigned int height, unsigned int width,
	glm::vec3 position /*= glm::vec3(0.0f, 0.0f, 0.0f)*/, glm::vec3 up /*= glm::vec3(0.0f, 1.0f, 0.0f)*/) :

	m_FOV(glm::radians(FOV_agle)), m_NearPlane(nearPlane), m_FarPlane(farPlane), m_Height(height),
	m_Width(width), m_Position(position), m_Target(position + glm::vec3(0.0f, 0.0f, 1.0f)), m_Up(up),
	m_Right(glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), up))),
	m_Locked(false), m_LockTo(position + glm::vec3(0.0f, 0.0f, 1.0f)), m_ActiveFramebuffer(0), m_ActiveOffset(0)

{
	Framebuffer fb(width, height);
	m_Framebuffers.push_back(fb);
	m_Offsets.push_back(glm::vec2(0, 0));
	m_Target = glm::normalize(m_Target - m_Position) + m_Position;
}

glm::mat4 Camera::matrix()
{
	glm::mat4 view = glm::mat4(1.0);
	glm::mat4 proj = glm::mat4(1.0);
	view = glm::lookAt(m_Position, m_Target, m_Up);
	proj = glm::perspective(m_FOV,
		static_cast<float>
		(m_Width) / (m_Height),
		m_NearPlane, m_FarPlane);
	return proj * view;
}

void Camera::farPlane(float far_plane)
{
	m_FarPlane = far_plane;
}

void Camera::nearPlane(float near_plane)
{
	m_NearPlane = near_plane;
}

void Camera::move(glm::vec3 dir)
{
	m_Position += dir;
	m_Target += dir;
}

void Camera::FOV(float angle)
{
	m_FOV = glm::radians(angle);
}

void Camera::lockTo(glm::vec3 target, bool lookAt /*= false*/)
{
	m_LockTo = target;
	if (lookAt)
	{
		m_Target = glm::normalize(target - m_Position) + position();
		m_Right = glm::normalize(glm::cross(orientation(), m_Up));
	}
	m_Locked = true;
}

void Camera::unlock()
{
	m_Locked = false;
}

void Camera::rotate(float angle, glm::vec3 pivot)
{
	glm::mat4 rotmat(1.0);
	if (m_Locked) 
	{
		rotmat = glm::rotate(rotmat, glm::radians(angle), pivot);
		glm::vec3 temp_pos = m_Position - m_LockTo;
		glm::vec3 temp_tar = m_Target - m_LockTo;

		temp_pos = glm::rotate(temp_pos, glm::radians(angle), pivot);
		temp_tar = glm::rotate(temp_tar, glm::radians(angle), pivot);

		glm::vec3 ltv = glm::normalize(temp_tar - temp_pos);
		if (glm::angle(ltv, m_Up) > PI * (174.0 / 180.0) || glm::angle(ltv, m_Up) < PI * (6.0 / 180.0))
		{
			return;
		}
		m_Position = temp_pos + m_LockTo;
		m_Target = temp_tar + m_LockTo;
		m_Right = rotmat * glm::vec4(m_Right, 1.0f);
	}
	else
	{
		rotmat = glm::rotate(rotmat, glm::radians(angle), pivot);
		glm::vec3 temp_tar = m_Target - m_Position;
		temp_tar = glm::rotate(temp_tar, glm::radians(angle), pivot);
		if (glm::angle(temp_tar, m_Up) > PI * (174.0/180.0) || glm::angle(temp_tar, m_Up) < PI * (6.0/180.0))
		{
			return;
		}
		m_Target = temp_tar + m_Position;
		m_Right = rotmat * glm::vec4(m_Right, 1.0f);
	}
}

void Camera::addFramebuffer()
{
	m_Framebuffers.push_back(Framebuffer(m_Width, m_Height));
}

void Camera::calculateOffsets(unsigned int number_of_samples)
{
// 	std::vector<glm::vec2> offsets;
// 	float x, y;
// 	int rootOfSize;
// 	rootOfSize = static_cast<int>(glm::ceil(glm::sqrt(number_of_samples)));
// 	for (int i = 0; i < number_of_samples; i++)
// 	{
// 		x = i % rootOfSize;
// 		x -= rootOfSize / 2.0F;
// 		y = i / rootOfSize;
// 		y -= rootOfSize / 2.0F;
// 		offsets.push_back(glm::vec2(x * (1.0f / (rootOfSize)) / m_Width, y * (1.0f / (rootOfSize)) / m_Height));
// 	}
// 	m_Offsets = offsets;
// 	m_ActiveOffset = 0;

	////////////////////////////////////////

	if (!number_of_samples)
		return;

	std::vector <glm::vec2> offsets;
	offsets.push_back(glm::vec2(0, 0));

	if (!(number_of_samples - 1))
	{
		m_Offsets = offsets;
		return;
	}

	glm::vec2 res(m_Width, m_Height);

	float x, y;
	const unsigned n = number_of_samples - 1;
	unsigned int k = 0;
	unsigned int r = 0;
	while (n > (2 * k * (k + 1)))
	{
		r = n - (2 * k * (k - 1));
		k++;
	}
	r = k ? r : n;
	for (int i = 0; i < k-1; i++)
	{
		float radius = (i * (0.5f / k)) + 0.0001f;
		for (int j = 0; j < 4 * i; j++)
		{
			glm::vec2 ent(radius, 0);
			ent = glm::rotate(ent, (j * PI) / i);
			offsets.push_back(ent/res);
		}
	}
	for (int i = 0; i < r; i++)
	{
		glm::vec2 ent(0.5001f, 0);
		ent = glm::rotate(ent, (i * PI) / r);
		offsets.push_back(ent/res);
	}
	std::cout << "generated " << offsets.size() << " offsets" << std::endl;
	m_Offsets = offsets;

}

void Camera::calculateOffsets()
{
	calculateOffsets(m_Framebuffers.size());
	m_ActiveOffset = m_ActiveFramebuffer;
}

void Camera::setActiveFramebuffer(unsigned int fbid)
{
	if (fbid > m_Framebuffers.size() - 1)
	{
		std::cout << "Framebuffer index out of range" << std::endl;
	}
	else
	{
		m_ActiveFramebuffer = fbid;
	}
}

void Camera::setActiveOffset(unsigned int offsetid)
{
	if (offsetid > m_Offsets.size() - 1)
	{
		std::cout << "Offset index out of range" << std::endl;
	}
	else
	{
		m_ActiveOffset = offsetid;
	}
}

void CameraInputControl(Camera& camera, GLFWwindow* window, float speed, float sensitivity, float d_time)
{
	static double posX, posY, initX, initY;
	static bool clicked = false;

	using namespace std;

	float spd = speed;
	float snstvt = sensitivity;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		spd = spd * 4.0;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
	{
		camera.lockTo(glm::vec3(0.0f, 0.0f, 0.0f));
		snstvt = sensitivity * 2;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE)
	{
		camera.unlock();
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.move(spd * d_time * camera.orientation());
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.move(-spd * d_time * camera.orientation());
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.move(-spd * d_time * camera.right());
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.move(spd * d_time * camera.right());
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		camera.move(spd * d_time * glm::cross(camera.right(), camera.orientation()));
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		camera.move(-spd * d_time * glm::cross(camera.right(), camera.orientation()));
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		camera.rotate(0.002f * snstvt, camera.right());
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		camera.rotate(-0.002f * snstvt, camera.right());
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		camera.rotate(0.002f * snstvt, camera.up());
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		camera.rotate(-0.002f * snstvt, camera.up());
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (!clicked)
		{
			glfwGetCursorPos(window, &initX, &initY);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			glfwSetCursorPos(window, camera.width() / 2.0, camera.height() / 2.0);
			clicked = true;
		}
		else
		{
			glfwGetCursorPos(window, &posX, &posY);
			glfwSetCursorPos(window, camera.width() / 2.0, camera.height() / 2.0);
			posX = glm::clamp((((posX / double(camera.width())) * 2.0) - 1.0) * snstvt, -5.0, 5.0);
			posY = glm::clamp((((posY / double(camera.height())) * 2.0) - 1.0) * snstvt * camera.height() / camera.width(), -5.0, 5.0);
			camera.rotate(-posY, camera.right());
			camera.rotate(-posX, camera.up());
		}
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && clicked)
	{
		glfwSetCursorPos(window, initX, initY);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		clicked = false;
	}
}
