#version 330

uniform sampler2DArray sampler;

smooth in vec4 fragpos;
smooth in vec4 fragnormal;
smooth in vec4 fraglight1;
smooth in vec4 fraglight2pos;
smooth in vec3 fragtex;

out vec4 outputColor;

void main() {
    vec4 texel = texture(sampler, fragtex);

    vec4 fraglight2dist = fraglight2pos - fragpos;
    
    float intensity = 0;
    intensity += max(-dot(fragnormal, fraglight1), 0.0);
    intensity += max(100*dot(fragnormal, normalize(fraglight2dist))
                     /dot(fraglight2dist, fraglight2dist), 0.0);
    
    outputColor.rgb = (texel * clamp(intensity, .2, 1.5)).rgb;
    outputColor.a = texel.a;
}
