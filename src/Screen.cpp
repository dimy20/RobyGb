#include "Screen.h"
void key_cb(GLFWwindow* window, int key, int scan_code, int action, int mods){
	Screen * screen = static_cast<Screen *>(glfwGetWindowUserPointer(window));
	screen->handle_keys(key, action);
};

void Screen::handle_keys(int key, int action){
	if(m_key_map.find(key) != m_key_map.end()){
		if(action == GLFW_PRESS && m_chip8->m_wait_key != NO_KEY_WAIT){
			m_chip8->key_pressed(m_key_map[key]);
			m_chip8->m_wait_key = NO_KEY_WAIT;
		}

		// store state
		int key_index = m_key_map[key];
		unsigned char * keys = m_chip8->keys();
		keys[key_index] = (action == GLFW_PRESS || action == GLFW_REPEAT) ? 1 : 0;
	}

};

void Screen::init(){
	 float vertices[] = {
        // display rect     // texture coords
         0.5f,  0.5f,	     1.0f, 0.0f,
         0.5f, -0.5f,	     1.0f, 1.0f,
        -0.5f, -0.5f,	     0.0f, 1.0f,
        -0.5f,  0.5f,	     0.0f, 0.0f
    };

    unsigned int indices[] = {0, 1, 3, 1, 2, 3};

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	GLenum err(glGetError());

    while (err != GL_NO_ERROR)
    {
		CHIP8_LOG("Opengl error: " << err);
        err = glGetError();
    }

}

void Screen::init_window(){
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	m_window = glfwCreateWindow(1024, 768, "Chip8 emulator", NULL, NULL);
	if(!m_window){
		glfwTerminate();
		return;
	};

	glfwMakeContextCurrent(m_window);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
	gladLoadGL();
	glViewport(0, 0, 1024, 768);
	glfwSetKeyCallback(m_window, key_cb);
	glfwSetWindowUserPointer(m_window, this);

	GLenum err(glGetError());

    while (err != GL_NO_ERROR)
    {
		CHIP8_LOG("Opengl error: " << err);
        err = glGetError();
    }
}

Screen::Screen(std::shared_ptr<Chip8> chip) : m_chip8(chip) {
	init_window();
	const char * vertex = "/etc/chip8/shaders/vert.glsl";
	const char * frag = "/etc/chip8/shaders/frag.glsl";
	m_shader = std::make_shared<Shader>(vertex, frag);
	init();
	init_texture(64, 32, chip->gfx());
	m_key_map[GLFW_KEY_1] = 1;
	m_key_map[GLFW_KEY_2] = 2;
	m_key_map[GLFW_KEY_3] = 3;
	m_key_map[GLFW_KEY_4] = 0x0c;
	m_key_map[GLFW_KEY_Q] = 4;
	m_key_map[GLFW_KEY_W] = 5;
	m_key_map[GLFW_KEY_E] = 6;
	m_key_map[GLFW_KEY_R] = 0x0d;
	m_key_map[GLFW_KEY_A] = 7;
	m_key_map[GLFW_KEY_S] = 8;
	m_key_map[GLFW_KEY_D] = 9;
	m_key_map[GLFW_KEY_F] = 0x0e;
	m_key_map[GLFW_KEY_Z] = 0x0a;
	m_key_map[GLFW_KEY_X] = 0;
	m_key_map[GLFW_KEY_C] = 0x0b;
	m_key_map[GLFW_KEY_V] = 0x0f;

};

void Screen::init_texture(int w, int h, unsigned char * data){
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	GLenum err(glGetError());

    while (err != GL_NO_ERROR)
    {
		CHIP8_LOG("Opengl error: " << err);
        err = glGetError();
    }
};

void Screen::update_texture(unsigned char * data){
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 64, 32, GL_RED, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
};

long Screen::render(){
	struct timespec frame_start, frame_end;
	clock_gettime(CLOCK_MONOTONIC, &frame_start);

	const unsigned char * gfx = m_chip8->gfx();
	unsigned char tmp[64 * 32];
	memcpy(tmp, gfx, 64 * 32);
	for(int i = 0; i < 32; i++){
		for(int j = 0; j < 64; j++) tmp[(i * 64) + j] *= 255;
	}
	update_texture(tmp);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	m_shader->use();

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	clock_gettime(CLOCK_MONOTONIC, &frame_end);
	GLenum err(glGetError());

    while (err != GL_NO_ERROR)
    {
		CHIP8_LOG("Opengl error: " << err);
        err = glGetError();
    }

	update_fps();
	long elapsed_ns = frame_end.tv_nsec - frame_start.tv_nsec;
	return elapsed_ns;
};

Screen::operator bool(){
	return !glfwWindowShouldClose(m_window);
};


void Screen::swap_buffers(){
	glfwPollEvents();    
	glfwSwapBuffers(m_window); //swap buffer
};

void Screen::close(){ glfwTerminate(); };
void Screen::clear(){ glClear(GL_COLOR_BUFFER_BIT);};

void Screen::update_fps(){
	if(m_prev_s == 0) m_prev_s = glfwGetTime();

    m_curr_s = glfwGetTime();
	m_frames_count++;

	double delta = m_curr_s - m_prev_s;
	if(delta > 1.0f){
		m_prev_s = m_curr_s;
		m_fps = (int)m_frames_count / (int)(delta);
		m_frames_count = 0;
	}
};
