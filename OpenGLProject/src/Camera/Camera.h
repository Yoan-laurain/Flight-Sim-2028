#pragma once

#include "Library/coreMinimal.h"

#define GLM_ENABLE_EXPERIMENTAL

struct GLFWwindow;

class Camera
{
	public:
		Camera(int width, int height, const Vec3<float>& position);

		void Update(GLFWwindow* window, double deltaTime);

		Vec3<float> m_Position;
		Mat4<float> m_ViewMatrix;
		Mat4<float> m_ProjectionMatrix;
		Mat4<float> m_ViewProjectionMatrix;
		Vec3<float> m_Rotation;
		Vec3<float> m_Up;

	private:
	
		float m_fov;
		float m_speed;
		float m_sensitivity;
		float m_nearPlane;
		float m_farPlane;

		double m_baseMouseX;
		double m_baseMouseY;

		bool m_firstClick;

		int m_width;
		int m_height;

		void HandleInputs(GLFWwindow* window, double deltaTime);
		void HandleRotation(GLFWwindow* window);
		void UpdateMatrix();
		void ResetToDefault();
};