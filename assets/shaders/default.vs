#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

uniform vec4 uColor;
uniform mat4 uTrans;
uniform mat4 uView;
uniform mat4 uProjection;

out vec4 color;

void main()
{
    gl_Position = uProjection * uView * uTrans * vec4(aPos, 1.0f);
    color = uColor;
}