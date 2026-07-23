#pragma once

#include <fstream>
#include <glad/gl.h>
#include <imgui.h>
#include <sstream>

// Simple red triangle mesh for testing OpenGL
struct ExampleMesh
{
	unsigned int program = 0;
	unsigned int vao = 0;
	unsigned int vbo = 0;

	// Initialize shaders and geometry
	void Init()
	{
		// Load Vertex Shader Source
		std::ifstream vsFile("Data/Shaders/example.vert");
		std::stringstream vsStream;
		vsStream << vsFile.rdbuf();
		std::string vsString = vsStream.str();
		const char* vs = vsString.c_str();

		// Load Fragment Shader Source
		std::ifstream fsFile("Data/Shaders/example.frag");
		std::stringstream fsStream;
		fsStream << fsFile.rdbuf();
		std::string fsString = fsStream.str();
		const char* fs = fsString.c_str();

		// Create and compile shaders
		unsigned int v = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(v, 1, &vs, nullptr);
		glCompileShader(v);

		unsigned int f = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(f, 1, &fs, nullptr);
		glCompileShader(f);

		// Link shaders
		program = glCreateProgram();
		glAttachShader(program, v);
		glAttachShader(program, f);
		glLinkProgram(program);
		glDeleteShader(v);
		glDeleteShader(f);

		// Geometry Data
		float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

		// Create geometry
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
		glEnableVertexAttribArray(0);
	}

	// Render shaders and geometry
	void Render() const
	{
		glUseProgram(program);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	// Destroy shaders and geometry
	void Destroy()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteProgram(program);
	}
};

// Simple dockable window for testing ImGui
struct ExampleWindow
{
	// Creates a window with given title, text, position and size
	static void Create(const char* title, const char* text, float posX, float posY, float sizeX, float sizeY)
	{
		ImGui::SetNextWindowPos(ImVec2(posX, posY), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(sizeX, sizeY), ImGuiCond_FirstUseEver);
		ImGui::Begin(title);
		ImGui::TextWrapped(text);
		ImGui::End();
	}
};
