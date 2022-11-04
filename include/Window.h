#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cstring>
#include "Shader.h"

class Gb_ppu;
class Window{
	friend Gb_ppu;
	public:
		Window() = default;
		void init();
		bool should_close(){ return glfwWindowShouldClose(m_window); };
		void swap_buffers();
		void draw();
		void update_texture(const unsigned char * data);

	private:
		void init_texture(int w, int h, unsigned char * data);
		void ppu_recv(const std::vector<unsigned char>& pixels);
	private:
		GLFWwindow * m_window = nullptr;
		unsigned int m_vbo;
		unsigned int m_vao;
		unsigned int m_ebo;
		Shader m_shader;
		unsigned int m_texture;

};

