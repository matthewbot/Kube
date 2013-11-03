#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tex;

uniform mat4 perspective;
uniform mat4 modelview;

const vec4 lightdir = normalize(vec4(.3, .5, -1, 0));

smooth out vec4 fragpos;
smooth out vec4 fragnormal;
smooth out vec4 fraglight1;
smooth out vec4 fraglight2pos;
smooth out vec3 fragtex;

void main() {
    fragpos = modelview*vec4(position, 1);
    gl_Position = perspective*fragpos;
    fragnormal = normalize(modelview*vec4(normal, 0));
    fraglight1 = normalize(modelview*lightdir);
    fraglight2pos = vec4(0, 0, 0, 1);
    fragtex = tex;
}
