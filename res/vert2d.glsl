#version 330

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 tex;

uniform mat4 perspective;
uniform mat4 modelview;

smooth out vec2 fragtex;

void main() {
    gl_Position = perspective*modelview*vec4(position, 0, 1);
    fragtex = tex;
}
