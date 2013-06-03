#version 330

uniform sampler2DArray sampler;

smooth in vec4 fragnormal;
smooth in vec3 fragtex;

out vec4 outputColor;

const vec4 lightdir = normalize(vec4(.2, 1, -.6, 0));

void main() {
    vec4 texel = texture(sampler, fragtex);
    float light = .7*max(dot(fragnormal, -lightdir), 0) + .3;
    outputColor.rgb = texel.rgb * light;
    outputColor.a = texel.a;
}
