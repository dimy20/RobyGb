#pragma once

#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "chip8.h"

class Screen{
	public:
		Screen(std::shared_ptr<Chip8> chip8);
		long render();
		operator bool();
		void close();
		void clear();
		void swap_buffers();
		GLFWwindow * window() const { return m_window; };
		int fps() const { return m_fps; };
		friend void key_cb(GLFWwindow* window, int key, int scancode, int action, int mods);
	private:
		void init();
		void init_window();
		void handle_keys(int key, int action);
		void init_texture(int w, int h, unsigned char * data);
		void update_texture(unsigned char * data);
		void update_fps();

	private:
		GLFWwindow * m_window;
		std::shared_ptr<Shader> m_shader;
		std::shared_ptr<Chip8> m_chip8;
		unsigned m_texture;
		unsigned int m_vao;
		unsigned int m_vbo;
		unsigned int m_ebo;
		std::unordered_map<int, unsigned char> m_key_map;
		int m_fps = 0;
		long m_prev_s = 0;
		long m_curr_s = 0;
		int m_frames_count = 0;
};
