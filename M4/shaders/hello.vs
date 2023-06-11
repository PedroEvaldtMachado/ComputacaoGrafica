#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 tex_coord;

out vec4 vertexColor;
out vec2 texCoord;

uniform mat4 model;

void main()
{
	gl_Position = model * vec4(position, 1.0f);
	vertexColor = vec4(color, 1.0);
	texCoord = vec2(tex_coord.x, 1 - tex_coord.y);
}
