#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTextureCoord;

uniform mat4 projection;
uniform mat4 model;

out vec2 text_coord;
out vec2 vertex_position;

void main()
{    
    gl_Position = vec4(aPos.xy, 0.0, 1.0);
	text_coord = aTextureCoord;
}
