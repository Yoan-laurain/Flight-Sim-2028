#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL

struct GLFWwindow;

class Camera
{
	public:
		Camera( int width, int height, glm::vec3 position);

		void Update(GLFWwindow* window );

		/*
		 * Reset the camera to the initial position, rotation and speed
		*/
		void Reset();

		glm::vec3 m_Position;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewProjectionMatrix;
		glm::vec3 m_Rotation;
		glm::vec3 m_Up;

		int m_Width;
		int m_Height;
		float m_NearPlane;
		float m_FarPlane;

		bool m_PlaneMode;

	private:

		float m_Fov;
		float m_Speed;
		float m_Sensitivity;

		double m_BaseMouseX;
		double m_BaseMouseY;

		bool m_FirstClick;

		void Inputs( GLFWwindow* window );
		void HandleRotation( GLFWwindow* window );
		void UpdateMatrix();
};