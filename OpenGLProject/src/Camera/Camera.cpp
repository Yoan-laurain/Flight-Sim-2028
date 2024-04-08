#include "Camera.h"
#include "../Config.h"
#include "../OpenGL/Shader/Shader.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

Camera::Camera(const int width, const int height, const glm::vec3 position)
	: m_Position(position)
	, m_Width(width)
	, m_Height(height)
	, m_NearPlane(BaseNearPlane)
	, m_FarPlane(BaseFarPlane)
	, m_Fov(BaseFOV)
	, m_BaseMouseX(0.0)
	, m_BaseMouseY(0.0)
	, m_PlaneMode(false)

{
	Reset();
}

void Camera::Update(GLFWwindow* window)
{
	Inputs(window);
	UpdateMatrix();
}

void Camera::Reset()
{
	m_Rotation = glm::vec3(0.f, 0.0f, -90.f);
	m_Up = glm::vec3(0.0f, 90.0f, 0.0f);
	m_Speed = BaseCameraSpeed;
	m_Sensitivity = 60.f;
}

void Camera::UpdateMatrix()
{
	m_ViewMatrix = glm::lookAt(m_Position, m_Position + glm::radians(m_Rotation), glm::radians(m_Up));
	m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), (float)m_Width / m_Height, m_NearPlane, m_FarPlane);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void Camera::Inputs(GLFWwindow* window)
{
	if(m_PlaneMode)
	{
		HandleRotation(window);
		m_Position += m_Speed * m_Rotation;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		// change orientation of the camera based on mouse movement
		HandleRotation(window);
		
		// Move forward
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			m_Position += m_Speed * glm::radians(m_Rotation);
		}
		// Move left
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			m_Position += m_Speed * -glm::normalize(glm::cross(glm::radians(m_Rotation), glm::radians(m_Up)));
		}
		// Move back
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			m_Position += m_Speed * -glm::radians(m_Rotation);
		}
		// Move right
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			m_Position += m_Speed * glm::normalize(glm::cross(glm::radians(m_Rotation), glm::radians(m_Up)));
		}
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && !m_FirstClick)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		m_FirstClick = true;

		// set the cursor back to the base position
		glfwSetCursorPos(window, m_BaseMouseX, m_BaseMouseY);
	}
}

void Camera::HandleRotation(GLFWwindow* window)
{
	if (m_FirstClick)
	{
		m_FirstClick = false;

		// Store the base mouse coordinates
		glfwGetCursorPos(window, &m_BaseMouseX, &m_BaseMouseY);

		// Prevents camera from jumping on the first click
		glfwSetCursorPos(window, m_Width / 2, m_Height / 2);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		return;
	}

	// Stores the coordinates of the cursor
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	// Resets the cursor to the center of the window
	glfwSetCursorPos(window, m_Width / 2, m_Height / 2);

	// Calculates the rotation of the camera
	const float rotX = m_Sensitivity * (float)(mouseY - (m_Height / 2)) / m_Height; 
	const float rotY = m_Sensitivity * (float)(mouseX - (m_Width / 2)) / m_Width; 

	// Rotates the camera around the up vector
	m_Rotation = glm::rotate(m_Rotation, glm::radians(-rotY), m_Up); 

	// Calculates the right vector
	const glm::vec3 right = glm::normalize(glm::cross(m_Rotation, m_Up)); 
	 
	// Rotates the camera around the right vector
	m_Rotation = glm::rotate(m_Rotation, glm::radians(-rotX), right);  
}