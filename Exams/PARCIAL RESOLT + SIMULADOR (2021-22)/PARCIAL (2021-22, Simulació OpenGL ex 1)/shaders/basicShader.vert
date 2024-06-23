#version 330 core

in vec3 vertex;
in vec3 color;

uniform mat4 TG, PM, VM;

out vec3 fcolor;

void main()  {
    fcolor = color;
    gl_Position = PM * VM * TG * vec4 (vertex, 1.0);
}
