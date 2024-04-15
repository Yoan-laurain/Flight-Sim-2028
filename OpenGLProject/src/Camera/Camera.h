#pragma once

#include "../Library/coreMinimal.h"

#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL

struct GLFWwindow;

class Camera
{
	public:
		Camera( int width, int height, const Vec3<float>& position);

		void Update(GLFWwindow* window,float deltaTime );

		/*
		 * Reset the camera to the initial position, rotation and speed
		*/
		void Reset();

		Vec3<float> m_Position;
		Mat4<float> m_ViewMatrix;
		Mat4<float> m_ProjectionMatrix;
		Mat4<float> m_ViewProjectionMatrix;
		Vec3<float> m_Rotation;
		Vec3<float> m_Up;

		int m_Width;
		int m_Height;
		float m_NearPlane;
		float m_FarPlane;

		bool m_PlaneMode;

	private:

		float m_Fov;
		float m_Speed{};
		float m_Sensitivity{};

		double m_BaseMouseX;
		double m_BaseMouseY;

		bool m_FirstClick{};

		void Inputs( GLFWwindow* window,float deltaTime );
		void HandleRotation( GLFWwindow* window );
		void UpdateMatrix();
};