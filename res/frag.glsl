#version 330

uniform sampler2DArray sampler;

smooth in vec4 fragnormal;
smooth in vec2 fragtex;

out vec4 outputColor;

const vec4 lightdir = normalize(vec4(.2, 1, -.6, 0));

void main() {
    vec4 texel = texture(sampler, vec3(fragtex, 1));
    float light = .9*max(dot(fragnormal, -lightdir), 0) + .1;
    outputColor.rgb = texel.rgb * light;
    outputColor.a = texel.a;
}
