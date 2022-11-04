#include "Shader.h"
void handle_error(unsigned int shader_id, const std::string& error_msg){
	int  success;
	char infoLog[512];
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

	if(!success){
		glGetShaderInfoLog(shader_id, 512, NULL, infoLog);
		std::cout << error_msg << std::endl << infoLog << std::endl;
	}
}

char * load_shaderv2(const std::string& filename){
	std::ifstream file(filename, std::ios::ate);

	if(!file){
		std::cerr << "failed to open file : " << filename << std::endl;
		exit(EXIT_FAILURE);
	}

	int size = file.tellg();
	file.seekg(0, std::ios::beg);

	char * buffer = static_cast<char*>(malloc(sizeof(char) * (size + 1)));
	if(!buffer)throw std::bad_alloc();

	if(!file.read(buffer, size)) std::cerr << "failed to read : " << filename << "\n";
	buffer[size] = '\0';

	return buffer;
};

static unsigned int build_fragment_shader(const std::string& filename){
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// load shader and set source
	char * fragment_shader_src = load_shaderv2(filename);
	glShaderSource(fragmentShader, 1, &fragment_shader_src, NULL);

	// compile
	glCompileShader(fragmentShader);

	handle_error(fragmentShader, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED");

	free(fragment_shader_src);
	return fragmentShader;
};

static unsigned int build_vertex_shader(const std::string& filename){
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char * vertex_shader_src = load_shaderv2(filename);

	glShaderSource(vertexShader, 1, &vertex_shader_src, NULL);
	glCompileShader(vertexShader);

	handle_error(vertexShader, "ERROR::SHADER::VERTEX::COMPILATION_FAILED");

	free(vertex_shader_src);
	return vertexShader;
};

void Shader::init(const std::string& vertex_path, const std::string& fragment_path){
	unsigned int vertex_shader = build_vertex_shader(vertex_path);
	unsigned int fragment_shader = build_fragment_shader(fragment_path);

	m_id = glCreateProgram();
	glAttachShader(m_id, vertex_shader);
	glAttachShader(m_id, fragment_shader);
	glLinkProgram(m_id);

	int success;
	char infoLog[512];
	glGetProgramiv(m_id, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(m_id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

};

void Shader::use() { glUseProgram(m_id); };
