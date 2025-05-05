#version 410 core

in vec2 TexCoord;
out vec4 color;

uniform sampler2D tex;

void main() {
  float value = texture2D(tex, TexCoord).r;
  color = vec4(vec3(value), 1.0f);
}