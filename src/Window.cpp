#include "Window.h"

void key_cb(GLFWwindow * window, int key, int scan_code, int action, int mods){
	(void)scan_code;
	(void)mods;
	Window * m_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
	auto& directions = m_window->m_directions;
	auto& buttons = m_window->m_buttons;
	// direction key
	if(directions.find(key) != directions.end()){


		if(action == GLFW_PRESS)
			directions[key] = 1;
		else if(action == GLFW_RELEASE)
			directions[key] = 0;


		// build P1's lower nible
		unsigned char input_line = directions[GLFW_KEY_S] << 3 |
								   directions[GLFW_KEY_W] << 2 |
								   directions[GLFW_KEY_A] << 1 |
								   directions[GLFW_KEY_D];

		m_window->m_joypad->set_directions(input_line);

	}else if(buttons.find(key) != buttons.end()){

		if(action == GLFW_PRESS)
			buttons[key] = 1;
		else if(action == GLFW_RELEASE)
			buttons[key] = 0;

		unsigned char input_line = buttons[GLFW_KEY_ENTER] << 3 |
								   buttons[GLFW_KEY_SPACE] << 2 |
								   buttons[GLFW_KEY_K] << 1 |
								   buttons[GLFW_KEY_L];


		m_window->m_joypad->set_buttons(input_line);
	}
};

void Window::build_keymap(){
	m_directions[GLFW_KEY_D]  = 0;   /* right */  m_buttons[GLFW_KEY_L]     = 0; /* A */
	m_directions[GLFW_KEY_A]  = 0;   /* left */   m_buttons[GLFW_KEY_K]     = 0; /* B */
	m_directions[GLFW_KEY_W]  = 0;    /* up */    m_buttons[GLFW_KEY_SPACE] = 0; /* Select */
	m_directions[GLFW_KEY_S]  = 0;   /* down */   m_buttons[GLFW_KEY_ENTER] = 0; /* Start */
}

void Window::init(Gb_joypad * joypad){
	m_joypad = joypad;
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	m_window = glfwCreateWindow(1024, 768, "RobyGb", NULL, NULL);


	if(!m_window){
	std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetWindowUserPointer(m_window, this);
	glfwSetKeyCallback(m_window, key_cb);
	glfwMakeContextCurrent(m_window);
	build_keymap();
	//glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetCursorPosCallback(window, mouse_callback);

	// verify that glad loads correctly
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cerr << "Failed to initialize GLAD" << std::endl;
		exit(1);
	}

	glViewport(0, 0, 1024, 768);

	m_shader.init("../shaders/vert.glsl", "../shaders/frag.glsl");

	// texture
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
		std::cerr << "Opengl error : " << err << std::endl;
        err = glGetError();
    }

	const int width = 160;
	const int height = 144;
	const int depth = 3;

	std::vector<unsigned char> texture(width * height * depth, 0);

	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			auto offset = ((i * width) + j) * depth;
			texture[offset] = 0;
			texture[offset + 1] = 0;
			texture[offset + 2] = 0;
		}
	};


	init_texture(width, height, &texture[0]);
};

void Window::draw(){
	m_shader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glBindVertexArray(m_vao);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	GLenum err(glGetError());
    while (err != GL_NO_ERROR)
    {
		std::cerr << "Opengl error : " << err << "\n";
        err = glGetError();
    }

};

void Window::swap_buffers(){
	glfwSwapBuffers(m_window); 
	glfwPollEvents();
	if(glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
		glfwSetWindowShouldClose(m_window, true);
	};
};

void Window::init_texture(int w, int h, unsigned char * data){
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	///glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	GLenum err(glGetError());

    while (err != GL_NO_ERROR)
    {
		std::cerr << "Opengl error : " << err << std::endl;
        err = glGetError();
    }
};

void Window::update_texture(const unsigned char * data){
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 160, 144, GL_RGB, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
};


void Window::ppu_recv(const std::vector<unsigned char>& pixels){
	update_texture(pixels.data());
};
