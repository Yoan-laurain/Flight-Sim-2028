#include "Camera.h"
#include "Config.h"
#include "Library/Math.h"
#include <GLFW/glfw3.h>

Camera::Camera(const int width, const int height, const Vec3<float>& position)
	: m_Position(position)
	, m_fov(BaseFOV)
	, m_speed(BaseCameraSpeed)
	, m_sensitivity(0.0)
	, m_nearPlane(BaseNearPlane)
	, m_farPlane(BaseFarPlane)
	, m_baseMouseX(0.0)
	, m_baseMouseY(0.0)
	, m_firstClick(true)
	, m_width(width)
	, m_height(height)

{
	ResetToDefault();
}

void Camera::Update(GLFWwindow* window, const double deltaTime)
{
	HandleInputs(window,deltaTime);
	UpdateMatrix();
}

void Camera::ResetToDefault()
{
	m_Rotation = Vec3(-200.f, -150.0f, -130.f);
	m_Up = Vec3(0.0f, 90.0f, 0.0f);
	m_speed = BaseCameraSpeed;
	m_sensitivity = 60.f;
}

void Camera::UpdateMatrix()
{
	m_ViewMatrix = Math::lookAtM4(m_Position, m_Position + Math::radians(m_Rotation), Math::radians(m_Up));
	m_ProjectionMatrix = Math::perspective<float>(Math::radians(m_fov),
	                                              static_cast<float>(m_width) / static_cast<float>(m_height),
	                                              m_nearPlane, m_farPlane);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void Camera::HandleInputs(GLFWwindow* window, const double deltaTime)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		HandleRotation(window);
		
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // Move forward
		{
			m_Position += m_speed * static_cast<float>(deltaTime) * Math::radians(m_Rotation);
		}
		
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // Move left
		{
			m_Position += m_speed * static_cast<float>(deltaTime) * -Math::Normalize(Math::cross(Math::radians(m_Rotation), Math::radians(m_Up)));
		}
		
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // Move back
		{
			m_Position += m_speed * static_cast<float>(deltaTime) * -Math::radians(m_Rotation);
		}
		
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // Move right
		{
			m_Position += m_speed * static_cast<float>(deltaTime) * Math::Normalize(Math::cross(Math::radians(m_Rotation), Math::radians(m_Up)));
		}
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && !m_firstClick)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_firstClick = true;

		if ( m_baseMouseX != 0.0 && m_baseMouseY != 0.0)
		{
			glfwSetCursorPos(window, m_baseMouseX, m_baseMouseY);
		}
	}
}

void Camera::HandleRotation(GLFWwindow* window)
{
	if (m_firstClick)
	{
		m_firstClick = false;

		// Store the base mouse coordinates
		glfwGetCursorPos(window, &m_baseMouseX, &m_baseMouseY);

		// Prevents camera from jumping on the first click
		glfwSetCursorPos(window, m_width / 2.0, m_height / 2.0);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		return;
	}

	// Stores the coordinates of the cursor
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	// Resets the cursor to the center of the window
	glfwSetCursorPos(window, m_width / 2.0, m_height / 2.0);
	
	const float rotX = m_sensitivity * static_cast<float>(mouseY - (m_height / 2.0)) /  static_cast<float>(m_height); 
	const float rotY = m_sensitivity * static_cast<float>(mouseX - (m_width / 2.0)) /  static_cast<float>(m_width); 
	
	m_Rotation = Math::rotate(m_Rotation, Math::radians(rotY), m_Up);
	Vec3<float> right = Math::Normalize(Math::cross(m_Rotation, m_Up));
	m_Rotation = Math::rotate(m_Rotation, Math::radians(rotX), right);
}