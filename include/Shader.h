#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>

class Shader{
	public:
		Shader() = default;
		void use();
		void init(const std::string& vertex_path, const std::string& fragment_path);
		unsigned int id() const { return m_id;  };
	private:
		unsigned int m_id;
};
