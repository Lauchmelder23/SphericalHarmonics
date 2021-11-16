add_executable(orbitals "main.cpp" "Model.cpp" "Shader.cpp" "Camera.cpp" "Orbital.cpp")

target_sources(orbitals PRIVATE 
	${CMAKE_SOURCE_DIR}/vendor/glad/src/glad.c
)

target_include_directories(orbitals PRIVATE 
	glfw
	glm
	${CMAKE_SOURCE_DIR}/vendor/glad/include
)

target_link_libraries(orbitals PRIVATE 
	glfw
	glm
)