add_executable(orbitals "main.cpp" "Model.cpp" "Shader.cpp" "Camera.cpp" "Orbital.cpp" "Axis.cpp" "CoordinateSystem.cpp")

# Add GLFW, GLM, GLAD and ImGui include directories to target
target_include_directories(orbitals PRIVATE 
	glfw
	glm
	${CMAKE_SOURCE_DIR}/vendor/glad/include
	${CMAKE_SOURCE_DIR}/vendor/imgui
)

# Link to glfw and glm (why?)
target_link_libraries(orbitals PRIVATE 
	glfw
	glm
)

# Find imgui base source files
file(GLOB IMGUI_SOURCES 
	${CMAKE_SOURCE_DIR}/vendor/imgui/*.cpp
)

# Add sources from 3rd-party libraries (glad & imgui & imgui backends)
target_sources(orbitals PRIVATE 
	${CMAKE_SOURCE_DIR}/vendor/glad/src/glad.c
	${IMGUI_SOURCES}
	${CMAKE_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_glfw.cpp
	${CMAKE_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_opengl3.cpp
)