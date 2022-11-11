cmake_minimum_required(VERSION 3.14)
project(GbEmu VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE debug)

include(GoogleTest)
add_subdirectory(glfw)

add_library(Gb_Emulator src/Gb_emulator.cpp
						src/Memory.cpp
						src/Gb_cartridge.cpp
						src/Gb_core.cpp
						src/Gb_ppu.cpp
						src/Gb_timer.cpp
						src/Window.cpp
						src/Shader.cpp
						src/glad.c)
target_compile_options(Gb_Emulator PRIVATE -Werror -Wall -Wextra)
target_include_directories(Gb_Emulator PUBLIC 
						  ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME}
				src/main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC 
						  ${CMAKE_CURRENT_SOURCE_DIR}/include
						  glfw)

target_link_libraries(${PROJECT_NAME} Gb_Emulator glfw GL)
