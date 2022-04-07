#version 460 core

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float     mixRatio = 0.0f;

in vec3 fColor;
in vec2 fTexCoords;

out vec4 FragColor;

void main() {
    FragColor = mix(texture(texture1, fTexCoords), texture(texture2, fTexCoords), mixRatio)
     * vec4(fColor, 1.0f);
}