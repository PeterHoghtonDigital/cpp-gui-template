#pragma once

#include <fstream>
#include <glad/gl.h>
#include <imgui.h>
#include <sstream>
#include <string>

namespace Examples
{
	// Simple triangle mesh for testing OpenGL
	struct TriangleMesh
	{
		// Geometry
		static constexpr float VERTICES[9] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

		GLuint Program = 0;
		GLuint VAO = 0;
		GLuint VBO = 0;

		// Initialize shaders and geometry
		TriangleMesh(const char* VertexShaderPath, const char* FragmentShaderPath) : Program(glCreateProgram())
		{
			// Load Vertex Shader
			std::ifstream VertexShaderFStream(VertexShaderPath);
			std::stringstream VertexShaderSStream;
			VertexShaderSStream << VertexShaderFStream.rdbuf();
			std::string VertexShaderString = VertexShaderSStream.str();
			const char* VertexShaderSource = VertexShaderString.c_str();

			// Load Fragment Shader
			std::ifstream FragmentShaderFStream(FragmentShaderPath);
			std::stringstream FragmentShaderSStream;
			FragmentShaderSStream << FragmentShaderFStream.rdbuf();
			std::string FragmentShaderString = FragmentShaderSStream.str();
			const char* FragmentShaderSource = FragmentShaderString.c_str();

			// Create and compile shaders
			GLuint VertexShader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(VertexShader, 1, &VertexShaderSource, nullptr);
			glCompileShader(VertexShader);

			GLuint FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(FragmentShader, 1, &FragmentShaderSource, nullptr);
			glCompileShader(FragmentShader);

			// Link shaders
			glAttachShader(Program, VertexShader);
			glAttachShader(Program, FragmentShader);
			glLinkProgram(Program);
			glDeleteShader(VertexShader);
			glDeleteShader(FragmentShader);

			// Create geometry
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
			glEnableVertexAttribArray(0);
		}

		// Render shaders and geometry
		void Render() const
		{
			glUseProgram(Program);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}

		// Destroy shaders and geometry
		void Destroy()
		{
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
			glDeleteProgram(Program);
		}
	};

	// Simple window for testing ImGui
	inline void CreateImGuiWindow(const char* Title, const char* Text, float PosX, float PosY, float SizeX, float SizeY)
	{
		ImGui::SetNextWindowPos(ImVec2(PosX, PosY), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(SizeX, SizeY), ImGuiCond_FirstUseEver);
		ImGui::Begin(Title);
		ImGui::TextWrapped("%s", Text);
		ImGui::End();
	}
}
