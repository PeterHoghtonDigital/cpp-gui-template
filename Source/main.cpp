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
	void GlfwErrorCallback(int Error, const char* Description)
	{
		std::cerr << "GLFW Error " << Error << ": " << Description << '\n';
	}
}

int main()
{
	// Initialize GLFW
	glfwSetErrorCallback(GlfwErrorCallback);
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW..." << '\n';
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
	const int WindowWidth = 1280;
	const int WindowHeight = 720;
	const char* WindowTitle = "Hello, World!";

	// Scale appropriately for 4K, Retina, and High-DPI displays
	float WindowScale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());

	// Create the application window
	GLFWwindow* Window = glfwCreateWindow(static_cast<int>(WindowWidth * WindowScale),
		static_cast<int>(WindowHeight * WindowScale), WindowTitle, nullptr, nullptr);
	if (!Window)
	{
		glfwTerminate();
		std::cerr << "Failed to create window..." << '\n';
		return -1;
	}
	glfwMakeContextCurrent(Window);

	// Initialize GLAD
	if (!gladLoadGL(glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD..." << '\n';
		return -1;
	}

	// Enable VSync
	glfwSwapInterval(1);

	// Initialize ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& IO = ImGui::GetIO();
	IO.ConfigFlags |=
		ImGuiConfigFlags_DockingEnable // Enables UI window docking
		| ImGuiConfigFlags_ViewportsEnable; // Enables moving UI windows outside the main application window
	IO.IniFilename = "imgui.ini"; // File used to save UI state, path is relative to build directory
	ImGui::StyleColorsDark(); // Enable dark mode
	ImGuiStyle& Style = ImGui::GetStyle();
	Style.ScaleAllSizes(WindowScale);
	Style.FontScaleDpi = WindowScale;
	IO.ConfigDpiScaleFonts = true;
	IO.ConfigDpiScaleViewports = true;
	if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		Style.WindowRounding = 0.0f;
		Style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
	if (!ImGui_ImplGlfw_InitForOpenGL(Window, true))
	{
		ImGui::DestroyContext();
		glfwDestroyWindow(Window);
		glfwTerminate();
		std::cerr << "Failed to initialize the ImGui GLFW backend..." << '\n';
		return -1;
	}
	if (!ImGui_ImplOpenGL3_Init("#version 410 core")) // Must match the OpenGL version in Initialize GLFW section
	{
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		glfwDestroyWindow(Window);
		glfwTerminate();
		std::cerr << "Failed to initialize the ImGui OpenGL backend..." << '\n';
		return -1;
	}

	// --- Create your shaders and geometry here ---
	Examples::TriangleMesh Mesh("Data/Shaders/example.vert", "Data/Shaders/example.frag");
	// ---------------------------------------------

	// Main update loop
	while (!glfwWindowShouldClose(Window))
	{
		// Process input
		glfwPollEvents();

		// Create the main UI window and dockspace
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuiWindowFlags Flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
								 | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
								 | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
								 | ImGuiWindowFlags_NoBackground;
		const ImGuiViewport* Viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(Viewport->Pos);
		ImGui::SetNextWindowSize(Viewport->Size);
		ImGui::SetNextWindowViewport(Viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("MainUIWindow", nullptr, Flags);
		ImGui::PopStyleVar(3);
		ImGui::DockSpace(ImGui::GetID("MainUIDockSpace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::End();

		// --- Create your ImGui windows here ---
		ImGui::ShowDemoWindow(); // Demo window containing common ImGui features, view the code to see how to
								 // implement various UI elements and functionality in your own windows!

		Examples::CreateImGuiWindow(
			"A Window!", "Hello from a window!", Viewport->Pos.x + 220, Viewport->Pos.y + 160, 400, 400);
		// --------------------------------------

		// Render the frame
		ImGui::Render();

		// Check if main window is minimized
		int ViewportWidth = 0;
		int ViewportHeight = 0;
		glfwGetFramebufferSize(Window, &ViewportWidth, &ViewportHeight);
		const ImDrawData* DrawData = ImGui::GetDrawData();
		const bool ShouldRenderMainWindow = (glfwGetWindowAttrib(Window, GLFW_ICONIFIED) == 0) && ViewportWidth > 0
											&& ViewportHeight > 0 && DrawData->DisplaySize.x > 0.0f
											&& DrawData->DisplaySize.y > 0.0f;

		if (ShouldRenderMainWindow)
		{
			glViewport(0, 0, ViewportWidth, ViewportHeight);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Background colour
			glClear(GL_COLOR_BUFFER_BIT);

			// --- Render your shaders and geometry here ---
			Mesh.Render();
			// ---------------------------------------------

			// Renders the UI windows inside the main application window
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(Window);
		}

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			// Renders any UI windows outside the main application window
			GLFWwindow* ActiveWindow = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(ActiveWindow);
		}
	}

	// --- Destroy your shaders and geometry here ---
	Mesh.Destroy();
	// ----------------------------------------------

	// Destroy ImGui and GLFW
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(Window);
	glfwTerminate();

	return 0;
}
