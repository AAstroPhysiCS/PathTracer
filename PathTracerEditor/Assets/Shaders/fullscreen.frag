#version 460 core

layout (binding = 0) uniform sampler2D u_OutputTexture;

in vec2 v_UV;
out vec4 FragColor;

void main() {
    FragColor = texture(u_OutputTexture, v_UV);
}