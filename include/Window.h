#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <unordered_map>
#include "Gb_joypad.h"
#include "Shader.h"

class Gb_ppu;
class Window{
	friend Gb_ppu;
	public:
		friend void key_cb(GLFWwindow * window, int key, int scan_code, int action, int mods);
		Window() = default;
		void init(Gb_joypad * joypad);
		bool should_close(){ return glfwWindowShouldClose(m_window); };
		void swap_buffers();
		void draw();
		void update_texture(const unsigned char * data);
	private:
		void init_texture(int w, int h, unsigned char * data);
		void ppu_recv(const std::vector<unsigned char>& pixels);
		void build_keymap();
	private:
		GLFWwindow * m_window = nullptr;
		unsigned int m_vbo;
		unsigned int m_vao;
		unsigned int m_ebo;
		Shader m_shader;
		unsigned int m_texture;
		std::unordered_map<int, int> m_buttons;
		std::unordered_map<int, int> m_directions;
		Gb_joypad * m_joypad;
};

