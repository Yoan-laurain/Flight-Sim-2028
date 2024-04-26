#include "Camera.h"
#include "../Config.h"
#include "Library/Math.h"

Camera::Camera(const int width, const int height, const Vec3<float>& position)
	: m_Position(position)
	, m_Width(width)
	, m_Height(height)
	, m_NearPlane(BaseNearPlane)
	, m_FarPlane(BaseFarPlane)
	, m_PlaneMode(false)
	, m_Fov(BaseFOV)
	, m_BaseMouseX(0.0)
	, m_BaseMouseY(0.0)

{
	Reset();
}

void Camera::Update(GLFWwindow* window,double deltaTime)
{
	Inputs(window,deltaTime);
	UpdateMatrix();
}

void Camera::Reset()
{
	m_Rotation = Vec3(-300.f, -150.0f, -90.f);
	m_Up = Vec3(0.0f, 90.0f, 0.0f);
	m_Speed = BaseCameraSpeed;
	m_Sensitivity = 60.f;
}

void Camera::UpdateMatrix()
{
	m_ViewMatrix = Math::lookAtM4(m_Position, m_Position + Math::radians(m_Rotation), Math::radians(m_Up));
	m_ProjectionMatrix = Math::perspective<float>(Math::radians(m_Fov), (float)m_Width / (float)m_Height, m_NearPlane, m_FarPlane);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void Camera::Inputs(GLFWwindow* window,double deltaTime)
{
	if(m_PlaneMode)
	{
		HandleRotation(window);
		m_Position += m_Speed * m_Rotation * deltaTime;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		// change orientation of the camera based on mouse movement
		HandleRotation(window);
		
		// Move forward
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			m_Position += m_Speed * deltaTime * Math::radians(m_Rotation);
		}
		// Move left
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			m_Position += m_Speed * deltaTime * -Math::Normalize(Math::cross(Math::radians(m_Rotation), Math::radians(m_Up)));
		}
		// Move back
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			m_Position += m_Speed * deltaTime * -Math::radians(m_Rotation);
		}
		// Move right
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			m_Position += m_Speed * deltaTime * Math::Normalize(Math::cross(Math::radians(m_Rotation), Math::radians(m_Up)));
		}
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && !m_FirstClick)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		m_FirstClick = true;

		if ( m_BaseMouseX != 0.0 && m_BaseMouseY != 0.0)
		{
			// set the cursor back to the base position
			glfwSetCursorPos(window, m_BaseMouseX, m_BaseMouseY);
		}
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
	m_Rotation = Math::rotate(m_Rotation, Math::radians(rotY), m_Up);

	// Calculates the right vector
	Vec3<float> right = Math::Normalize(Math::cross(m_Rotation, m_Up));
	 
	// Rotates the camera around the right vector
	m_Rotation = Math::rotate(m_Rotation, Math::radians(rotX), right);
}