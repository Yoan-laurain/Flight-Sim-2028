#pragma once

#include <GL/glew.h>

class VertexArray;
class IndexBuffer;
class Shader;

class Renderer
{
	public:

		Renderer();
		~Renderer();

		/*
		 * @brief Clear the screen
		*/
		void Clear() const;

		/*
		 * Draw the elements contained in the VertexArray using the IndexBuffer and the Shader
		*/
		void Draw( const VertexArray& va, const IndexBuffer& ib, const Shader& shader ) const;

		/*
		 * @brief Handle OpenGL errors and print them to the console
		 * If you wand to see the exact location of the error, put a breakpoint in this function
		 */
		static void HandleError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
};