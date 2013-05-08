#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex;

uniform mat4 perspective;
uniform mat4 modelview;

smooth out vec4 fragnormal;
smooth out vec2 fragtex;

void main() {
    gl_Position = perspective*modelview*vec4(position, 1);
    fragnormal = normalize(vec4(normal, 0));
    fragtex = tex;
}
