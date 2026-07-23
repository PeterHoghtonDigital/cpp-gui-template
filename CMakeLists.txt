# ================================
# CMake Build Settings
# ================================
# This CMakeLists.txt defines the build configuration for the C++ project.
# It configures C++20 support, exports compile commands for clangd, automatically
# includes source files, copies shaders, fetches third-party libraries, 
# and defines platform and build configuration macros.

cmake_minimum_required(VERSION 3.25)

# Set the application name and version
project(cpp-gui-template VERSION 1.0.0 LANGUAGES C CXX)

# Set C++ Language Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export to compile_commands.json (for clangd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Force Clang to show coloured diagnostics output
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-fcolor-diagnostics)
endif()

# Fetch Third Party Libraries
include(FetchContent)

# GLFW
set(glfw_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External/glfw")
set(glfw_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/External/glfw")
set(glfw_SUBBUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/External/glfw-subbuild")

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
    GIT_PROGRESS   TRUE
    SOURCE_DIR     ${glfw_SOURCE_DIR}
    BINARY_DIR     ${glfw_BINARY_DIR}
    SUBBUILD_DIR   ${glfw_SUBBUILD_DIR}
)

# Disable GLFW examples, tests, and documentation
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(glfw)

# GLAD
set(glad_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External/glad")
add_library(glad STATIC "${glad_SOURCE_DIR}/src/gl.c")
target_include_directories(glad PUBLIC "${glad_SOURCE_DIR}/include")

# Dear ImGui
set(imgui_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External/imgui")
set(imgui_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/External/imgui")
set(imgui_SUBBUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/External/imgui-subbuild")

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.92.8-docking
    GIT_PROGRESS   TRUE
    SOURCE_DIR     ${imgui_SOURCE_DIR}
    BINARY_DIR     ${imgui_BINARY_DIR}
    SUBBUILD_DIR   ${imgui_SUBBUILD_DIR}
)
FetchContent_MakeAvailable(imgui)

# Find system OpenGL package
find_package(OpenGL REQUIRED)

# Add Source files
file(GLOB_RECURSE SRC CONFIGURE_DEPENDS "Source/*.cpp")
set(IMGUI_SRC 
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
)

# Create Executable
add_executable(cpp-gui-template
    ${SRC}
    ${IMGUI_SRC}
)

# Add Include Directories
target_include_directories(cpp-gui-template PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/"
    "${imgui_SOURCE_DIR}"
    "${imgui_SOURCE_DIR}/backends"
)

# Link Libraries
target_link_libraries(cpp-gui-template PRIVATE 
    glfw 
    glad
    OpenGL::GL
)

# Link macOS Required Libraries (macOS Only)
if(APPLE)
    find_library(COCOA Cocoa)
    find_library(IOKIT IOKit)
    find_library(COREVIDEO CoreVideo)
    target_link_libraries(cpp-gui-template ${COCOA} ${IOKIT} ${COREVIDEO})
endif()

# Add GLSL Shader Files
file(GLOB_RECURSE SHADER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Data/Shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/Data/Shaders/*.frag"
    "${CMAKE_CURRENT_SOURCE_DIR}/Data/Shaders/*.comp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Data/Shaders/*.tesc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Data/Shaders/*.tese"
    "${CMAKE_CURRENT_SOURCE_DIR}/Data/Shaders/*.geom"
)

# Copy GLSL Shaders to Build directory
foreach(SHADER_SOURCE IN LISTS SHADER_SOURCES)
    file(RELATIVE_PATH SHADER_RELATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Data/Shaders" "${SHADER_SOURCE}")
    configure_file("${SHADER_SOURCE}" "${CMAKE_CURRENT_BINARY_DIR}/Data/Shaders/${SHADER_RELATIVE_PATH}" COPYONLY)
endforeach()

# Define Platform Macros
if(WIN32)
    target_compile_definitions(cpp-gui-template PRIVATE WINDOWS)
elseif(APPLE)
    target_compile_definitions(cpp-gui-template PRIVATE MAC)
elseif(LINUX)
    target_compile_definitions(cpp-gui-template PRIVATE LINUX)
endif()

# Define Configuration Macros
target_compile_definitions(cpp-gui-template PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:RelWithDebInfo>:RELEASE>
    $<$<CONFIG:Release>:RELEASE>
)