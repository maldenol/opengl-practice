#version 460 core
layout (location = 0) in vec3 vertex;

void main() {
    gl_Position = vec4(vertex.xyz, 1.0);
}