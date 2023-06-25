// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
#version 450
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texc;
layout (location = 3) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 finalColor;
out vec3 fragPos;
out vec3 scaledNormal;
out vec2 texCoord;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
	finalColor = color;
	scaledNormal = mat3(transpose(inverse(model))) * normal;
	fragPos = vec3(model * vec4(position, 1.0));
	texCoord = vec2(texc.x, 1 - texc.y);
}