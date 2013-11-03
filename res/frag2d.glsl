#version 330

uniform sampler2D sampler;

smooth in vec2 fragtex;

out vec4 outputColor;

void main() {
    outputColor = texture(sampler, fragtex);
//  outputColor = vec4(1, 0, 0, 1);
}
