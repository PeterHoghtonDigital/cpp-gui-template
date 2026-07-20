#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

#include "examples.h" // Simple OpenGL mesh and ImGui window examples for testing

namespace
{
	void glfw_error_callback(int error, const char* description)
	{
		std::cerr << "GLFW Error " << error << ": " << description << '\n';
	}
}

int main()
{
	// Initialize GLFW
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << '\n';
		return -1;
	}
	// Note: OpenGL version 4.1 is the maximum supported by macOS
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef MAC
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Set the application window width, height and title
	const int window_width = 1280;
	const int window_height = 720;
	const char* window_title = "Hello, World!";

	// Create the application window
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, window_title, nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		std::cerr << "Failed to create window" << '\n';
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLAD
	if (!gladLoadGL(glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD\n";
		return -1;
	}

	// Enable VSync
	glfwSwapInterval(1);

	// Initialize ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |=
		ImGuiConfigFlags_DockingEnable // Enables UI window docking
		| ImGuiConfigFlags_ViewportsEnable; // Enables moving UI windows outside the main application window
	io.IniFilename = "imgui.ini"; // File used to save UI state, path is relative to build directory
	ImGui::StyleColorsDark(); // Enable dark mode
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 410 core"); // Must match the OpenGL version in Initialize GLFW section

	// --- Create your shaders and geometry here ---
	ExampleMesh exampleMesh;
	exampleMesh.Init();
	// ---------------------------------------------

	// Main update loop
	while (!glfwWindowShouldClose(window))
	{
		// Process input
		glfwPollEvents();

		// Create the main UI window and dockspace
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
								 | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
								 | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("MainUIWindow", nullptr, flags);
		ImGui::PopStyleVar(3);
		ImGui::DockSpace(ImGui::GetID("MainUIDockSpace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::End();

		// --- Create your ImGui windows here ---
		ExampleWindow::Create(
			"A Window!", "Hello from a window!", viewport->Pos.x + 220, viewport->Pos.y + 160, 400, 400);

		ExampleWindow::Create(
			"Another Window!", "Hello from another window!", viewport->Pos.x + 660, viewport->Pos.y + 160, 400, 400);
		// --------------------------------------

		// Render the frame
		ImGui::Render();
		int viewport_width = 0;
		int viewport_height = 0;
		glfwGetFramebufferSize(window, &viewport_width, &viewport_height);
		glViewport(0, 0, viewport_width, viewport_height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.00f); // Background colour
		glClear(GL_COLOR_BUFFER_BIT);

		// --- Render your shaders and geometry here ---
		exampleMesh.Render();
		// ---------------------------------------------

		// Renders the UI windows inside the main application window
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			// Renders any UI windows outside the main application window
			GLFWwindow* backup = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup);
		}
		glfwSwapBuffers(window);
	}

	// --- Destroy your shaders and geometry here ---
	exampleMesh.Destroy();
	// ----------------------------------------------

	// Destroy ImGui and GLFW
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
